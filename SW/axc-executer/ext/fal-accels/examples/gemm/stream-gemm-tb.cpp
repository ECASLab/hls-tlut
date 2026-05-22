/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example stream-gemm-tb.cpp
 * This is the testbench implementation of the GEMM accelerator
 */

#include "stream-gemm-tb.hpp"

#include <time.h>

#include <iostream>
#include <opencv2/opencv.hpp>

#include "linear.hpp"
#include "profile/degreesoffreedom.hpp"
#include "profile/profiler.hpp"

/* #define VERBOSE_OUTPUT */

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
  const int rows{rep ? kRowsSingle : Rows};
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
  constexpr int kNumSamples = 20;
  /* Buffers */
  float in_mat_a[kBatchSizeTb][kInputsTb];
  float in_mat_b[kInputsTb][kOutputsTb];
  float sw_result[kBatchSizeTb][kOutputsTb];
  float hw_result[kBatchSizeTb][kOutputsTb];
  DataType hw_in_mat_a[kRows][kColumns];
  DataType hw_in_mat_b[kRows][kColumns];
  DataType hw_out_result[kRows][kColumns];
  int execution_counter{0};

  int ret{0};
  time_t t;

  PayloadType payload{0};

  /* Streaming ports */
  StreamPort stream_input{}, stream_output{};

  /* Execution params */
  DECL_EXE_PARAMS_TB;

  /* Capabilities */
  DECL_CAPS_TB;

  constexpr float limit_factor{0.5f};
  constexpr float min_value = -limit_factor;
  constexpr float max_value = limit_factor;

  /* Evaluation */
  int error_evaluation = (1 << fal::profile::Profiler::ABS_MEAN_STD) |
                         (1 << fal::profile::Profiler::ABS_MIN_MAX) |
                         (1 << fal::profile::Profiler::MSE) |
                         (1 << fal::profile::Profiler::RMSE) |
                         (1 << fal::profile::Profiler::PSNR) |
                         (1 << fal::profile::Profiler::FROBENIUS) |
                         (1 << fal::profile::Profiler::HISTOGRAM);
  fal::profile::Profiler profiler{error_evaluation};

  srand(Q_SEED);

  /* Generate a matrix which is 16 times bigger to give an example of
     concurrency */
  cv::Mat mat_a(kBatchSizeTb, kInputsTb, CV_32F, in_mat_a[0]);
  cv::Mat mat_b(kInputsTb, kOutputsTb, CV_32F, in_mat_b[0]);
  cv::Mat mat_sw_result(kBatchSizeTb, kOutputsTb, CV_32F, sw_result[0]);

  for (int i = 0; i < kNumSamples; ++i) {
    std::cout << "-- Filling data --" << std::endl;
    cv::randu(mat_a, min_value, max_value);
    cv::randu(mat_b, min_value, max_value);
#ifdef VERBOSE_OUTPUT
    std::cout << "Matrix A: " << std::endl;
    ama::utils::print_matrices<float, kBatchSizeTb, kInputsTb>(in_mat_a);
    std::cout << "Matrix B: " << std::endl;
    ama::utils::print_matrices<float, kInputsTb, kOutputsTb>(in_mat_b);
#endif
    /* -------------------------------------------------------- */
    /* --------------------- Software run --------------------- */
    /* -------------------------------------------------------- */
    std::cout << "-- Starting SW simulation --" << std::endl;
    mat_sw_result = mat_a * mat_b;
    std::cout << "-- Finished HW simulation --" << std::endl;
#ifdef VERBOSE_OUTPUT
    std::cout << "Obtained SW (exact) result" << std::endl;
    ama::utils::print_matrices<float, kBatchSizeTb, kOutputsTb>(sw_result);
#endif

    /* -------------------------------------------------------- */
    /* --------------------- Hardware run --------------------- */
    /* -------------------------------------------------------- */
    /* -- Write properties -- */
    WRITE_EXE_PARAM_TB(rows, kRows);
    WRITE_EXE_PARAM_TB(columns, kColumns);
    WRITE_EXE_PARAM_TB(load_matrix_mask, static_cast<int>(MatMasks::A) |
                                             static_cast<int>(MatMasks::B));
    WRITE_EXE_PARAM_TB(operation, static_cast<int>(MatOperations::FMA));
    WRITE_EXE_PARAM_TB(en_accumulation, 0);
    WRITE_EXE_PARAM_TB(en_activation, 0);

    /* -- Execute -- */
    WRITE_EXE_PARAM_TB(execution_mode,
                       static_cast<int>(ExecutionModes::CONFIG));
    EXECUTE_ACCEL;

    /* -- Read properties back -- */
#ifdef VERBOSE_OUTPUT
    READ_PROPS;
#endif

    /* -- GEMM -- */
    std::cout << "-- Starting HW simulation --" << std::endl;

    /* The A-X,Y, B-X are usually not vectorised */
    int total_xb{
        static_cast<int>(static_cast<float>(kOutputsTb) / kColumns + 0.5f)};
    int total_iyb{
        static_cast<int>(static_cast<float>(kBatchSizeTb) / kRows + 0.5f)};
    /* B - Y must be replicated across the vector matrix */
    int total_oyb{
        static_cast<int>(static_cast<float>(kInputsTb) / kRowsSingle + 0.5f)};

    for (int ayb{0}; ayb < total_iyb; ++ayb) {
      /* For accumulation purposes, this should be the last one */
      /* The Y position in A matches C - we need to be careful of handling the
       * last byb */
      for (int bxb{0}; bxb < total_xb; ++bxb) {
        /* The index in X of B matches the X index of C */
        for (int byb{0}; byb < total_oyb; ++byb) {
          /* To accumulate, we require to move the Y position in B and X
           * position in A */
          int axb{byb};
          bool accumulate = (byb + 1) != total_oyb;
          WRITE_EXE_PARAM_TB(en_accumulation, accumulate);
          /* Load A matrix */
#ifdef VERBOSE_OUTPUT
          std::cout << "A at:";
#endif
          extract_hw_matrix<kBatchSizeTb, kInputsTb, kRows, kColumns>(
              in_mat_a, hw_in_mat_a, axb, ayb, false);
#ifdef VERBOSE_OUTPUT
          std::cout << "HW Matrix A: " << std::endl;
          ama::utils::print_matrices<DataType, kRows, kColumns>(hw_in_mat_a);
#endif
          UploadMatrix<kRows, kColumns>(hw_in_mat_a, stream_input);

          /* Load B matrix - replicated */
#ifdef VERBOSE_OUTPUT
          std::cout << "B at:";
#endif
          extract_hw_matrix<kInputsTb, kOutputsTb, kRows, kColumns>(
              in_mat_b, hw_in_mat_b, bxb, byb, true);
#ifdef VERBOSE_OUTPUT
          std::cout << "HW Matrix B: " << std::endl;
          ama::utils::print_matrices<DataType, kRows, kColumns>(hw_in_mat_b);
#endif
          UploadMatrix<kRows, kColumns>(hw_in_mat_b, stream_input);

          /* -- Run with data -- */
          EXECUTE_ACCEL;
          ++execution_counter;

          /* B Must be replicated in all blocks */
          if (accumulate) continue;

          /* In case of not accumulating, the result must be retrived */
          DownloadMatrix<kRows, kColumns>(stream_output, hw_out_result);
#ifdef VERBOSE_OUTPUT
          if (!accumulate) std::cout << "C at:";
#endif
          extract_sw_matrix<kBatchSizeTb, kOutputsTb, kRows, kColumns>(
              hw_out_result, hw_result, bxb, ayb);
#ifdef VERBOSE_OUTPUT
          std::cout << "Obtained Partial HW result" << std::endl;
          ama::utils::print_matrices<DataType, kRows, kColumns>(hw_out_result);
#endif
        }
      }
    }

    std::cout << "-- Finished HW simulation --" << std::endl;
#ifdef VERBOSE_OUTPUT
    std::cout << "Obtained HW result" << std::endl;
    ama::utils::print_matrices<float, kBatchSizeTb, kOutputsTb>(hw_result);
#endif

    profiler.Register(hw_result[0], sw_result[0], kBatchSizeTb, kOutputsTb);
  }
  /* -------------------------------------------------------- */
  /* --------------------- Evaluation ----------------------- */
  /* -------------------------------------------------------- */
  /* DoFs */
  fal::profile::DegreesOfFreedomReg dof{};
  dof.Register("Core", TOSTRING(Q_CORE));
  dof.Register("Seed", Q_SEED);
  dof.Register("Big Matrix Inputs", kInputsTb);
  dof.Register("Big Matrix Outputs", kOutputsTb);
  dof.Register("Big Matrix Batch", kBatchSizeTb);
  dof.Register("PEs", Q_PES);
  dof.Register("PE Rows", kRowsSingle);
  dof.Register("PE Columns", kColumns);
  dof.Register("Datatype Width", Q_BW);
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
