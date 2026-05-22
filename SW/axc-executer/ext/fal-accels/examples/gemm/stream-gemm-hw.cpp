/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example stream-gemm-hw.cpp
 * This is the hardware implementation of the GEMM accelerator
 */

#include <ap_fixed.h>

#include <hw-ops/MatrixMultiplyAdd.hpp>
#include <hw-ops/VectorMatrix.hpp>

#include "stream-gemm.hpp"

static constexpr int kIntegerIntType{kColumns};
using IntType = ap_fixed<Q_BW + kIntegerIntType, kIntegerIntType + Q_INT>;

void execute(DataType A[kRows][kColumns], DataType B[kRows][kColumns],
             DataType C[kRows][kColumns]) {
  static const IntType alpha{Q_O * 2};
  static DataType D[kRows][kColumns] = {0.f};

  /* Execute the PEs */
  using Engine = ama::hw::operators::MatrixMultiplyAdd<DataType, Q_O, Q_O>;
  ama::hw::ParallelMatrixOperator<Q_PES, Q_PES, Engine>::Execute(A, B, C, D);

  /* Copy to D to C, scaling back */
execute_rows_buffer:
  for (int row{0}; row < kRows; ++row) {
  execute_cols_buffer:
    for (int col{0}; col < kColumns; ++col) {
      IntType tmp{D[row][col]};
      tmp *= alpha;
      C[row][col] = tmp;
    }
  }
}

/**
 * Sends the output matrix. rows and columns are unused
 */
void retrieve_data(StreamPort& stream_output, DataType C[kRows][kColumns],
                   PropertyPort en_accumulation, DimensionPort rows,
                   DimensionPort columns) {
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

  if (en_accumulation) {
    return;
  }

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_retrieve_copy_row:
  for (int i{0}; i < kRows; ++i) {
  data_retrieve_copy_col:
    for (int j{0}; j < kColumns; ++j) {
      output_mat[i * kColumns + j] = C[i][j];
      /* Since it is not accumulating, reset the output */
      C[i][j] = 0.f;
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
               DataType B[kRows][kColumns], DataType C[kRows][kColumns],
               PropertyPort load_matrix_mask, DimensionPort rows,
               DimensionPort columns) {
  if (stream_input.empty()) return;

  /* Check matrices to transmit */
  ap_uint<1> capture_A = load_matrix_mask.range(static_cast<uint>(MatIdx::A),
                                                static_cast<uint>(MatIdx::A));
  ap_uint<1> capture_B = load_matrix_mask.range(static_cast<uint>(MatIdx::B),
                                                static_cast<uint>(MatIdx::B));
  ap_uint<1> capture_C = load_matrix_mask.range(static_cast<uint>(MatIdx::C),
                                                static_cast<uint>(MatIdx::C));

  /* Load matrices */
  if (capture_A) {
    receive_matrix(stream_input, A, rows, columns);
  }

  if (capture_B) {
    receive_matrix(stream_input, B, rows, columns);
  }

  if (capture_C) {
    receive_matrix(stream_input, C, rows, columns);
  }
}
