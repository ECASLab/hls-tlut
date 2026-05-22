/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include "multiplication.hpp"

void multiplication_accel_top(
    /* Stream ports */
    StreamPort &input, StreamPort &output,

    /* Runtime params */
    PropertyType &rows, PropertyType &columns,

    /* Capabilities */
    PropertyType &min_rows, PropertyType &min_columns, PropertyType &max_rows,
    PropertyType &max_columns) {
  /* Set a static buffer to hold the result. A = A, B = BT */
  static DataType A[kRows][kColumns] = {0.f};
  static DataType B[kColumns][kColumns] = {0.f};
  static DataType C[kRows][kColumns] = {0.f};

  /* Setup */
  /* FIXME: improve check to be a multiple of kMinRows and kMinCols */
  rows = rows > PropertyType{kMinRows} ? rows : PropertyType{kMinRows};
  columns =
      columns > PropertyType{kMinColumns} ? columns : PropertyType{kMinColumns};

  /* Capabilities */
  min_rows = PropertyType{kMinRows};
  min_columns = PropertyType{kMinColumns};
  max_rows = kMinRows; // FIXME: TLAST Output must be signaled properly
  max_columns = 0xFFFF;

  /* Avoid stuff */
  if (input.empty()) return;

  // TODO, filterout the number of rows and columns for continuous execution
  for (int row_b{0}; row_b < rows; row_b += kRows) {
#pragma HLS LOOP_TRIPCOUNT avg=2
    for (int col_b{0}; col_b < columns; col_b += kColumns) {
#pragma HLS LOOP_TRIPCOUNT avg=400
      /* Compute accumulation */
      Load(input, A, B);
      Execute(A, B, C);
    }
    /* After the last accumulation and the last row, finish */
    WriteBack(output, C);
  }
}
