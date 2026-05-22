/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include <ap_fixed.h>
#include <ratio>

#include <approximate/arithmetic.hpp>
#include <exact/arithmetic.hpp>
#include <hw-ops/MatrixElementWise.hpp>

#include "dataflow-addition.hpp"

void execute(DataType A[kRows][kColumns], DataType B[kRows][kColumns],
             DataType C[kRows][kColumns]) {
#if Q_AMETHOD == EXACT
  using Op = axc::arithmetic::exact::Add<DataType>;
#elif Q_AMETHOD == LSBDROP
  using Op =
      axc::arithmetic::approximate::lsbdrop::Add<DataType, Q_BW, Q_INT, Q_NBA>;
#elif Q_AMETHOD == LSBOR
  using Op =
      axc::arithmetic::approximate::lsbor::Add<DataType, Q_BW, Q_INT, Q_NBA>;
#else
#error "Unknown approximation method"
#endif

  /* Create operation engines */
  using Engine =
      ama::hw::operators::MatrixElementWise<DataType, kRows, kColumns, Op>;

  static Engine engine = Engine{};
  engine.Execute(A, B, C);
}

/**
 * Sends the output matrix.
 */
void retrieve_data(StreamPort& stream_output, DataType B[kRows][kColumns],
                   DimensionPort rows, DimensionPort columns) {
  /* Size of the window delivered by the accelerator */
  constexpr int kNumOutputElems = kRows * kColumns;
  /* Size of the output in current run */
  const int kNumImageElems = rows * columns;
  /* Total number of runs */
  const int kNumExecutions = kNumImageElems / kNumOutputElems;

  static DataType output_mat[kRowsPad * kColumns] = {0.f};

  /* Execution counter */
  static int execution_counter{0};
  ++execution_counter;

  /* Determine whether it is the final execution */
  bool final_execution = kNumExecutions == execution_counter;

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_retrieve_copy_row:
  for (int i{0}; i < kRows; ++i) {
  data_retrieve_copy_col:
    for (int j{0}; j < kColumns; ++j) {
      output_mat[i * kColumns + j] = B[i][j];
    }
  }

data_retrieve_packet:
  for (int e{0}; e < kNumOutputElems; e += kTotalPacketsPerPayload) {
    StreamPayload spayload{};
    bool end_of_window = kNumOutputElems <= (e + kTotalPacketsPerPayload);
  data_retrieve_elements:
    for (int p{0}; p < kTotalPacketsPerPayload; ++p) {
      const int upper{Q_BW * (p + 1) - 1};
      const int lower{Q_BW * p};

      spayload.data.range(upper, lower) = output_mat[e + p].V;
    }
    spayload.keep = -1;
    spayload.last = final_execution && end_of_window;
    spayload.user.range(0, 0) = end_of_window;
    stream_output.write(spayload);
  }
}

/**
 * Routine to get a matrix
 * @param stream_input input stream
 * @param mat matrix to load
 * @param rows matrix to load - currently unused
 * @param columns matrix to load - currently unused
 */
inline static void receive_matrix(StreamPort& stream_input,
                                  DataType mat[kRows][kColumns],
                                  DimensionPort rows, DimensionPort columns) {
  constexpr int kNumInputElems{kRows * kColumns};

  static DataType input_mat[kRowsPad * kColumns];

data_stream_loop_packet:
  for (int e{0}; e < kNumInputElems; e += kTotalPacketsPerPayload) {
    StreamPayload spayload{};
    spayload = stream_input.read();
  data_stream_loop_element:
    for (int p{0}; p < kTotalPacketsPerPayload; ++p) {
      const int upper{Q_BW * (p + 1) - 1};
      const int lower{Q_BW * p};

      /* Compute row and col */
      input_mat[e + p].V = spayload.data.range(upper, lower);
    }
    if (spayload.last) break;
  }

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_stream_copy_row:
  for (int i{0}; i < kRows; ++i) {
  data_stream_copy_col:
    for (int j{0}; j < kColumns; ++j) {
      mat[i][j] = input_mat[i * kColumns + j];
    }
  }
}

/**
 * Retrieves the input matrices. rows and columns are unused
 */
void load_data(StreamPort& stream_input, DataType A[kRows][kColumns],
               DimensionPort rows, DimensionPort columns) {
  if (stream_input.empty()) return;

  receive_matrix(stream_input, A, rows, columns);
}
