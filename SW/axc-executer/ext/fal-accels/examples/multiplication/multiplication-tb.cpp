/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "multiplication.hpp"

#include <iostream>
#include <utils/print_matrices.hpp>

#define VERBOSE_OUTPUT

/**
 * Uploads the image from a batch array to the HLS Stream
 * @tparam Rows rows of the HW matrix
 * @tparam Columns columns of the HW matrix
 * @param input_batch batch to copy to the stream
 * @param stream_input stream port to emit the data
 */
template <int Rows, int Columns, int BatchRows, int BatchColumns>
static void UploadMatrix(const DataType input_batch[Rows][Columns],
                         StreamPort &stream_input, 
                         const int xo = 0, const int yo = 0,
                         bool last = false) {
  PayloadType payload{0};
  int payl_elems{0};

#ifdef VERBOSE_OUTPUT
  std::cout << "Uploading Matrix" << std::endl;
#endif
  for (int y{0}; y < BatchRows; ++y) {
    for (int x{0}; x < BatchColumns; ++x) {
      /* If the elements are complete */
      if (kDataPerPayload == payl_elems) {
        StreamType spayload{};
        spayload.data = payload;
        spayload.keep = -1;
        spayload.last = 0;
        stream_input.write(spayload);
        payl_elems = 0;
        payload = 0;
      }

      int upper{kDataWidth * (payl_elems + 1) - 1};
      int lower{kDataWidth * payl_elems};
      payload.range(upper, lower) = input_batch[y + yo][x + xo].V;
      ++payl_elems;
    }
  }

  /* Last write */
  if (payl_elems != 0) {
    StreamType spayload{};
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
  const int kTotalPackets = Rows * Columns / kDataPerPayload;
  int x{0};
  int y{0};
  int packets{0};

  StreamType spayload{};
#ifdef VERBOSE_OUTPUT
  std::cout << "Before reading - Output Stream Size: " << stream_output.size()
            << std::endl;
#endif

  do {
    spayload = stream_output.read();

    /* Read the packets */
    for (int p{0}; p < kDataPerPayload; ++p) {
      /* Read and set */
      int upper = kDataWidth * (p + 1) - 1;
      int lower = kDataWidth * p;
      output_batch[y][x].V = spayload.data.range(upper, lower);
      /* Check boundaries */
      if (++x >= Columns) {
        x = 0;
        if (++y >= Rows) {
          break;
        }
      }
    }

  } while (!spayload.last);
#ifdef VERBOSE_OUTPUT
  std::cout << "After reading - Output Stream Size: " << stream_output.size()
            << std::endl;
#endif
}

#ifdef VALIDATE_EXECUTE
int main(int, char **) {
  DataType A[kRows][kColumns];
  DataType B[kColumns][kColumns];
  DataType C[kRows][kColumns] = {0.f};

  /* Fill */
  for (int row{0}; row < kColumns; ++row) {
    for (int col{0}; col < kColumns; ++col) {
      A[row % kRows][col] = 0.05f * row * col;
      B[row][col] = 0.05f * row * col;
      C[row % kRows][col] = 0.f;
    }
  }

  std::cout << "A: " << std::endl;
  ama::utils::print_matrices<DataType, kRows, kColumns>(A);
  std::cout << "B: " << std::endl;
  ama::utils::print_matrices<DataType, kColumns, kColumns>(B);

  /* Test execute */
  Execute(A, B, C);

  std::cout << "C: " << std::endl;
  ama::utils::print_matrices<DataType, kRows, kColumns>(C);

  return 0;
}

#else
int main(int, char **) {
  static constexpr int kRowsTB = 2;
  static constexpr int kColumnsTB = kColumns * 100;

  DataType A[kRowsTB][kColumnsTB];
  DataType B[kColumnsTB][kColumns];
  DataType C[kRowsTB][kColumns] = {0.f};

  StreamPort accel_input;
  StreamPort accel_output;

  PropertyType rows{kRowsTB};
  PropertyType columns{kColumnsTB};

  PropertyType min_rows{0};
  PropertyType min_columns{0};
  PropertyType max_rows{0};
  PropertyType max_columns{0};

  /* Get information */
  multiplication_accel_top(
      /* Stream ports */
      accel_input, accel_output,

      /* Runtime params */
      rows, columns,

      /* Capabilities */
      min_rows, min_columns, max_rows, max_columns);

  std::cout << "Configurations:"
            << "\nRows: " << rows << "\nColumns: " << columns
            << "\nMin Rows: " << min_rows << "\nMin Columns: " << min_columns
            << "\nMax Rows: " << max_rows << "\nMax Columns: " << max_columns
            << std::endl;

  /* Fill */
  for (int row{0}; row < kColumns; ++row) {
    for (int col{0}; col < kColumnsTB; ++col) {
      A[row % kRowsTB][col % kColumnsTB] = 0.0005f * (row + 0.01f) * col;
      B[col % kColumnsTB][row % kColumns] = 0.0005f * (row + 0.01f) * col;
      C[row % kRowsTB][col % kColumns] = 0.f;
    }
  }

  std::cout << "A: " << std::endl;
  ama::utils::print_matrices<DataType, kRowsTB, kColumnsTB>(A);
  std::cout << "B: " << std::endl;
  ama::utils::print_matrices<DataType, kColumnsTB, kColumns>(B);

  /* Interleave data */
#ifdef INTERLEAVE
  for (int yo{0}; yo < kColumns; yo += kColumns) {
    for (int xo{0}; xo < kColumnsTB; xo += kColumns) {
      if (yo < kRowsTB) {
        UploadMatrix<kRowsTB, kColumnsTB, kRows, kColumns>(A, accel_input, xo, yo, false);
      }
      UploadMatrix<kColumnsTB, kColumns, kColumns, kColumns>(B, accel_input, yo, xo,
                                         false);  // TODO: fix this later
    }
  }
#else
  UploadMatrix<kRowsTB, kColumnsTB, kRowsTB, kColumnsTB>(A, accel_input, 0, 0, false);
  UploadMatrix<kColumnsTB, kColumns, kColumnsTB, kColumns>(B, accel_input, 0, 0,
                                                           false);  // TODO: fix this later
#endif

  /* Test execute */
  multiplication_accel_top(
      /* Stream ports */
      accel_input, accel_output,

      /* Runtime params */
      rows, columns,

      /* Capabilities */
      min_rows, min_columns, max_rows, max_columns);

  std::cout << "C: " << std::endl;
  std::cout << "State of input: "
            << (accel_input.size() == 0 ? std::string("empty") : ("with " + std::to_string(accel_input.size()) + " elements"))
            << std::endl;
  DownloadMatrix<kRowsTB, kColumns>(accel_output, C);
  ama::utils::print_matrices<DataType, kRowsTB, kColumns>(C);

  return 0;
}
#endif
