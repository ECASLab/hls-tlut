/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include "dataflow-addition-tb.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <ratio>

#include "linear.hpp"
#include "profile/degreesoffreedom.hpp"
#include "profile/profiler.hpp"

#define VERBOSE_OUTPUT

/**
 * Uploads the image from a batch array to the HLS Stream
 * @tparam Rows rows of the HW matrix
 * @tparam Columns columns of the HW matrix
 * @param input_batch batch to copy to the stream
 * @param stream_input stream port to emit the data
 */
template <int Rows, int Columns>
static void UploadMatrix(const DataType input_batch[Rows][Columns],
                         StreamPort &stream_input, bool last = false) {
  PayloadType payload{0};
  int payl_elems{0};

#ifdef VERBOSE_OUTPUT
  std::cout << "Uploading Matrix" << std::endl;
#endif
  for (int y{0}; y < Rows; ++y) {
    for (int x{0}; x < Columns; ++x) {
      /* If the elements are complete */
      if (kTotalPacketsPerPayload == payl_elems) {
        StreamPayload spayload{};
        spayload.data = payload;
        spayload.keep = -1;
        spayload.last = 0;
        stream_input.write(spayload);
        payl_elems = 0;
        payload = 0;
      }

      int upper{Q_BW * (payl_elems + 1) - 1};
      int lower{Q_BW * payl_elems};
      payload.range(upper, lower) = input_batch[y][x].V;
      ++payl_elems;
    }
  }

  /* Last write */
  if (payl_elems != 0) {
    StreamPayload spayload{};
    spayload.data = payload;
    spayload.keep = -1;
    spayload.last = last;
    stream_input.write(spayload);
    payl_elems = 0;
  }
#ifdef VERBOSE_OUTPUT
  std::cout << "Stream size: " << stream_input.size() << " Batch size: " << Rows
            << "x" << Columns << " Q_BW: " << Q_BW << std::endl;
#endif
}

/**
 * Downloads the image from the HLS Stream to a batch array
 * @tparam Rows rows of the HW matrix
 * @tparam Columns columns of the HW matrix
 * @param stream_output stream port to receive the data from
 * @param output_batch batch to copy from the stream
 */
template <int Rows, int Columns>
static void DownloadMatrix(StreamPort &stream_output,
                           DataType output_batch[Rows][Columns]) {
  const int kTotalPackets = Rows * Columns / kTotalPacketsPerPayload;
  int x{0};
  int y{0};
  int packets{0};

  StreamPayload spayload{};
#ifdef VERBOSE_OUTPUT
  std::cout << "Before reading - Output Stream Size: " << stream_output.size()
            << std::endl;
#endif

  do {
    spayload = stream_output.read();

    /* Read the packets */
    for (int p{0}; p < kTotalPacketsPerPayload; ++p) {
      /* Read and set */
      int upper = Q_BW * (p + 1) - 1;
      int lower = Q_BW * p;
      output_batch[y][x].V = spayload.data.range(upper, lower);
      /* Check boundaries */
      if (++x >= kColumns) {
        x = 0;
        if (++y >= kRows) {
          break;
        }
      }
    }

  } while (!spayload.user.range(0, 0));
#ifdef VERBOSE_OUTPUT
  std::cout << "After reading - Output Stream Size: " << stream_output.size()
            << std::endl;
#endif
}

/**
 * Makes matches from matrix from software to hardware
 * @tparam RowsTb rows of the testbench matrix
 * @tparam ColumnsTb columns of the testbench matrix
 * @tparam Rows rows of the HW matrix
 * @tparam Columns columns of the HW matrix
 * @param sw_mat software exact matrix
 * @param hw_mat hardware matrix
 * @param xb index of the X block
 * @param yb index of the Y block
 * @param rep replication flag
 */
template <int RowsTb, int ColumnsTb, int Rows, int Columns>
static void extract_hw_matrix(const float sw_mat[RowsTb][ColumnsTb],
                              DataType hw_mat[Rows][Columns], const int xb,
                              const int yb, const bool rep) {
  const int offset_x{xb * Columns};
  const int rows{rep ? kRows : Rows};
  const int offset_y{yb * rows};
#ifdef VERBOSE_OUTPUT
  std::cout << "(" << offset_y << "," << offset_x << ")" << std::endl;
#endif
  /* Copy elements within the offset */
  for (int i{0}; i < Rows; ++i) {
    for (int j{0}; j < Columns; ++j) {
      const int elem_x{offset_x + j};
      const int elem_y{offset_y + (i % rows)};

      if (elem_x >= ColumnsTb || elem_y >= RowsTb) {
        hw_mat[i][j] = 0.f;
      } else {
        hw_mat[i][j] = sw_mat[elem_y][elem_x];
      }
    }
  }
}

/**
 * Makes matches from matrix from hardware to software
 * @tparam RowsTb rows of the testbench matrix
 * @tparam ColumnsTb columns of the testbench matrix
 * @tparam Rows rows of the HW matrix
 * @tparam Columns columns of the HW matrix
 * @param sw_mat software exact matrix
 * @param hw_mat hardware matrix
 * @param xb index of the X block
 * @param yb index of the Y block
 * @param rep replication flag
 */
template <int RowsTb, int ColumnsTb, int Rows, int Columns>
static void extract_sw_matrix(const DataType hw_mat[Rows][Columns],
                              float sw_mat[RowsTb][ColumnsTb], const int xb,
                              const int yb) {
  const int offset_x{xb * Columns};
  const int offset_y{yb * Rows};
#ifdef VERBOSE_OUTPUT
  std::cout << "(" << offset_y << "," << offset_x << ")" << std::endl;
#endif
  /* Copy elements within the offset */
  for (int i{0}; i < Rows; ++i) {
    for (int j{0}; j < Columns; ++j) {
      const int elem_x{offset_x + j};
      const int elem_y{offset_y + i};

      if (elem_x < ColumnsTb && elem_y < RowsTb) {
        sw_mat[elem_y][elem_x] = hw_mat[i][j];
      }
    }
  }
}

int main(int argc, char **argv) {
  using kMin = std::ratio<Q_MIN_NUM, Q_MIN_DEN>;
  using kMax = std::ratio<Q_MAX_NUM, Q_MAX_DEN>;
  constexpr int kNumSamples = 20;

  /* Buffers */
  float in_mat_a[kRowsTb][kColsTb];
  float in_mat_b[kRowsTb][kColsTb];
  float sw_result[kRowsTb][kColsTb];
  float hw_result[kRowsTb][kColsTb];
  DataType hw_in_mat_a[kRows][kColumns];
  DataType hw_in_mat_b[kRows][kColumns];
  DataType hw_out_result[kRows][kColumns];
  int execution_counter{0};

  int ret{0};
  time_t t;

  PayloadType payload{0};

  /* Streaming ports */
  StreamPort STREAM_BINARY_INPUT_A{}, STREAM_BINARY_INPUT_B{},
      STREAM_BINARY_OUTPUT{};

  /* Execution params */
  DECL_EXE_PARAMS_TB;

  /* Capabilities */
  DECL_CAPS_TB;

  constexpr float min_value = kMin::num / kMin::den;
  constexpr float max_value = kMax::num / kMax::den;

  /* Evaluation */
  int error_evaluation = (1 << fal::profile::Profiler::ABS_MEAN_STD) |
                         (1 << fal::profile::Profiler::ABS_MIN_MAX) |
                         (1 << fal::profile::Profiler::MSE) |
                         (1 << fal::profile::Profiler::RMSE) |
                         (1 << fal::profile::Profiler::PSNR) |
                         (1 << fal::profile::Profiler::HISTOGRAM);

  const double kHistMax = 2 * max_value;
  const int kHistBins = 100 * int(kHistMax);
  fal::profile::Profiler profiler{error_evaluation, kHistBins, kHistMax};

  srand(Q_SEED);

  /* Generate a matrix which is 16 times bigger to give an example of
     concurrency */
  cv::Mat mat_a(kRowsTb, kColsTb, CV_32F, in_mat_a[0]);
  cv::Mat mat_b(kRowsTb, kColsTb, CV_32F, in_mat_b[0]);
  cv::Mat mat_sw_result(kRowsTb, kColsTb, CV_32F, sw_result[0]);

  for (int i = 0; i < kNumSamples; ++i) {
    std::cout << "-- Filling data --" << std::endl;
    cv::randu(mat_a, min_value, max_value);
    cv::randu(mat_b, min_value, max_value);
#ifdef VERBOSE_OUTPUT
    std::cout << "Matrix A: " << std::endl;
    ama::utils::print_matrices<float, kRowsTb, kColsTb>(in_mat_a);
    std::cout << "Matrix B: " << std::endl;
    ama::utils::print_matrices<float, kRowsTb, kColsTb>(in_mat_b);
#endif
    /* -------------------------------------------------------- */
    /* --------------------- Software run --------------------- */
    /* -------------------------------------------------------- */
    std::cout << "-- Starting SW simulation --" << std::endl;
    mat_sw_result = mat_a + mat_b;
    std::cout << "-- Finished SW simulation --" << std::endl;
#ifdef VERBOSE_OUTPUT
    std::cout << "Obtained SW (exact) result" << std::endl;
    ama::utils::print_matrices<float, kRowsTb, kColsTb>(sw_result);
#endif

    /* -------------------------------------------------------- */
    /* --------------------- Hardware run --------------------- */
    /* -------------------------------------------------------- */
    int total_xb{
        static_cast<int>(static_cast<float>(kColsTb) / kColumns + 0.5f)};
    int total_yb{static_cast<int>(static_cast<float>(kRowsTb) / kRows + 0.5f)};
    int total_elems{total_xb * total_yb * kMinElements};

    /* -- Write properties -- */
    WRITE_EXE_PARAM_TB(elements, total_elems);

    /* -- Execute -- */
    WRITE_EXE_PARAM_TB(execution_mode,
                       static_cast<int>(ExecutionModes::CONFIG));
    EXECUTE_ACCEL;

    /* -- Read properties back -- */
#ifdef VERBOSE_OUTPUT
    READ_PROPS;
#endif

    /* -- Addition 1D/2D -- */
    std::cout << "-- Starting HW simulation --" << std::endl;

    std::cout << "Uploading data..." << std::endl;

    /* Run over windows */
    for (int yb{0}; yb < total_yb; ++yb) {
      for (int xb{0}; xb < total_xb; ++xb) {
        /* -- Extract matrix from SW input and upload -- */
        extract_hw_matrix<kRowsTb, kColsTb, kRows, kColumns>(
            in_mat_a, hw_in_mat_a, xb, yb, false);
        UploadMatrix<kRows, kColumns>(hw_in_mat_a, STREAM_BINARY_INPUT_A);
        extract_hw_matrix<kRowsTb, kColsTb, kRows, kColumns>(
            in_mat_b, hw_in_mat_b, xb, yb, false);
        UploadMatrix<kRows, kColumns>(hw_in_mat_b, STREAM_BINARY_INPUT_B);
#ifdef VERBOSE_OUTPUT
        std::cout << "HW Matrix A: " << std::endl;
        ama::utils::print_matrices<DataType, kRows, kColumns>(hw_in_mat_a);
        std::cout << "HW Matrix B: " << std::endl;
        ama::utils::print_matrices<DataType, kRows, kColumns>(hw_in_mat_b);
#endif
      }
    }

    std::cout << "Executing..." << std::endl;
    /* -- Run with data -- */
    EXECUTE_ACCEL;
    execution_counter = total_elems / kMinElements;

    std::cout << "Downloading..." << std::endl;
    for (int yb{0}; yb < total_yb; ++yb) {
      for (int xb{0}; xb < total_xb; ++xb) {
        /* In case of not accumulating, the result must be retrived */
        DownloadMatrix<kRows, kColumns>(STREAM_BINARY_OUTPUT, hw_out_result);
        extract_sw_matrix<kRowsTb, kColsTb, kRows, kColumns>(hw_out_result,
                                                             hw_result, xb, yb);
#ifdef VERBOSE_OUTPUT
        std::cout << "Obtained Partial HW result" << std::endl;
        ama::utils::print_matrices<DataType, kRows, kColumns>(hw_out_result);
#endif
      }
    }

    std::cout << "-- Finished HW simulation --" << std::endl;
#ifdef VERBOSE_OUTPUT
    std::cout << "Obtained HW result" << std::endl;
    ama::utils::print_matrices<float, kRowsTb, kColsTb>(hw_result);
#endif

    profiler.Register(hw_result[0], sw_result[0], kRowsTb, kColsTb);
  }
  /* -------------------------------------------------------- */
  /* --------------------- Evaluation ----------------------- */
  /* -------------------------------------------------------- */
  /* DoFs */
  fal::profile::DegreesOfFreedomReg dof{};
  dof.Register("Core", TOSTRING(Q_CORE));
  dof.Register("Seed", Q_SEED);
  dof.Register("Big Matrix Rows", kRowsTb);
  dof.Register("Big Matrix Cols", kColsTb);
  dof.Register("Big Matrix Batch", kMinElements);
  dof.Register("PEs", kMinElements);
  dof.Register("Datatype Width", Q_BW);
  dof.Register("Approximate bits in adder", Q_NBA);
  dof.Register("Approximation method", Q_AMETHOD);
  dof.Register("Datatype Integer", Q_INT);
  dof.Register("Datatype Fraction", Q_BW - Q_INT);
  dof.Register("Execution Counter", execution_counter / kNumSamples);
  dof.Print();

  /* Print error evaluation */
  profiler.Print();

  /* -- Fix for co-sim -- */
  EXECUTE_ACCEL;

  return ret;
}
