/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "multiplication.hpp"

#include <hw-ops/MatrixMultiplyAdd.hpp>

#include <utils/print_matrices.hpp>

void GetSubMatrix(const DataType X[kRows][kColumns],
                  DataType Xp[kSizePE][kSizePE], const int offset_x,
                  const int offset_y) {
#pragma HLS inline
  for (int j = 0; j < kSizePE; ++j) {
#pragma HLS UNROLL
    for (int i = 0; i < kSizePE; ++i) {
#pragma HLS UNROLL
      Xp[j][i] = X[j + offset_y][i + offset_x];
    }
  }
}

void SetSubMatrix(const DataType Xp[kSizePE][kSizePE],
                  DataType X[kRows][kColumns], const int offset_x,
                  const int offset_y) {
#pragma HLS inline
#pragma HLS UNROLL
  for (int j = 0; j < kSizePE; ++j) {
#pragma HLS UNROLL
    for (int i = 0; i < kSizePE; ++i) {
      X[j + offset_y][i + offset_x] = Xp[j][i];
    }
  }
}

void Execute(const DataType A[kRows][kColumns],
             const DataType B[kColumns][kColumns],
             DataType C[kRows][kColumns]) {
#pragma HLS inline off
  static_assert(kSizePE == 2, "Cannot deal with non 2x2 PEs");
  static_assert(kColumns % kSizePE == 0,
                "Cannot deal with kColumns not multiple of PE size");
  static_assert(kRows % kSizePE == 0,
                "Cannot deal with kRows not multiple of PE size");

  using Engine =
      ama::hw::operators::MatrixMultiplyAdd<DataType, kSizePE, kSizePE>;

  /* Sectorise matrices. Risks for 2x2 PEs */
execute_pe_row_jump:
  for (int row_b{0}; row_b < kColumns; row_b += kSizePE) {
  execute_pe_col_jump:
    for (int col_a{0}; col_a < kColumns; col_a += kSizePE) {
      /* PE Matrices */
      static DataType Ap[kSizePE][kSizePE] = {0.f};
      static DataType Bp[kSizePE][kSizePE] = {0.f};
      static DataType Cp[kSizePE][kSizePE] = {0.f};
      static DataType Dp[kSizePE][kSizePE] = {0.f};

      /* Clear matrices */
    clear_pe_matrix_port_rows:
      for (int row{0}; row < kSizePE; ++row) {
      clear_pe_matrix_port_cols:
        for (int col{0}; col < kSizePE; ++col) {
          Ap[row][col] = 0;
          Bp[row][col] = 0;
          Cp[row][col] = 0;
          Dp[row][col] = 0;
        }
      }

      /* Copy the matrices */
      GetSubMatrix(A, Ap, col_a, 0);
      GetSubMatrix(B, Bp, row_b, col_a);
      GetSubMatrix(C, Cp, row_b, 0);

      /* Execute */
      Engine engine{};
      engine.Execute(Ap, Bp, Cp, Dp);

      /* Write back matrix - implicit addition */
      SetSubMatrix(Dp, C, row_b, 0);
    }
  }
}

template <int Rows, int Columns>
inline static void ReceiveMatrix(StreamPort& stream_input,
                                 DataType mat[Rows][Columns]) {
#pragma HLS inline /* You need this ON for consistency */
  constexpr int kNumInputElems{Rows * Columns};
  static DataType input_mat[Rows * Columns];

  // FIXME if (stream_input.empty()) return;

data_stream_loop_packet:
  for (int e{0}; e < kNumInputElems; e += kDataPerPayload) {
    StreamType spayload{};
    spayload = stream_input.read();
  data_stream_loop_element:
    for (int p{0}; p < kDataPerPayload; ++p) {
      const int upper{kDataWidth * (p + 1) - 1};
      const int lower{kDataWidth * p};

      /* Compute row and col */
      input_mat[e + p].V = spayload.data.range(upper, lower);
    }
    if (spayload.last) break;
  }

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_stream_copy_row:
  for (int i{0}; i < Rows; ++i) {
  data_stream_copy_col:
    for (int j{0}; j < Columns; ++j) {
      mat[i][j] = input_mat[i * Columns + j];
    }
  }
}

/**
 * Retrieves the input matrices. rows and columns are unused
 */
void Load(StreamPort& stream_input, DataType A[kRows][kColumns],
          DataType B[kColumns][kColumns]) {
#pragma HLS inline /* You need this ON for consistency */

  // TODO: Add masking

  /* Load matrices */
  ReceiveMatrix<kRows, kColumns>(stream_input, A);
  ReceiveMatrix<kColumns, kColumns>(stream_input, B);
}

/**
 * Sends the output matrix. rows and columns are unused
 */
void WriteBack(StreamPort& stream_output, DataType C[kRows][kColumns]) {
#pragma HLS inline off
  // TODO: extend for accumulation

  /* Size of the window delivered by the accelerator */
  constexpr int kNumOutputElems = kRows * kColumns;

  /* It is safe to assume that the columns fit into the stream */
  static DataType output_mat[kRows * kColumns] = {0.f};

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_retrieve_copy_row:
  for (int i{0}; i < kRows; ++i) {
  data_retrieve_copy_col:
    for (int j{0}; j < kColumns; ++j) {
      output_mat[i * kColumns + j] = C[i][j];
      /* Since it is not accumulating, reset the output */
      C[i][j] = DataType{0.f};
    }
  }

data_retrieve_packet:
  for (int e{0}; e < kNumOutputElems; e += kDataPerPayload) {
    StreamType spayload{};
  data_retrieve_elements:
    for (int p{0}; p < kDataPerPayload; ++p) {
      const int upper{kDataWidth * (p + 1) - 1};
      const int lower{kDataWidth * p};

      spayload.data.range(upper, lower) = output_mat[e + p].V;
    }
    spayload.keep = -1;
    spayload.last = (e + kDataPerPayload >= kNumOutputElems);
    stream_output.write(spayload);
  }
}
