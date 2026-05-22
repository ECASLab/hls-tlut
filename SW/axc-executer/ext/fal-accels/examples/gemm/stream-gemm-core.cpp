/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example stream-gemm-core.cpp
 * This is the core of the stream GEMM accelerator
 */

#include <hw-ops/MatrixMultiplyAdd.hpp>
#include <hw-ops/VectorMatrix.hpp>

#include "stream-gemm.hpp"

/* --- Accelerator routines --- */

/**
 * Define the GEMM capabilities of the accelerator. This function
 * writes ranges of the operands, supported capabilities, accelerator type
 * and acceleration version.
 * Capabilities available:
 * - rows
 * - columns
 * - scaling
 * - masking
 * - accumulation
 * - activation
 * - def_scaling
 * - operations
 * - num_cores
 * - datatype
 * - num_bits_integer
 * - num_bits_fraction
 * - accel_type
 * - accel_version
 * @param function name
 */
DEF_SET_MATRIX_CAPABILITIES(gemm_capabilities) {
  WRITE_RANGE_CAP(rows, kRows, kMatrixMaxSize);
  WRITE_RANGE_CAP(columns, kColumns, kMatrixMaxSize);
  WRITE_RANGE_CAP(scaling, kScaling, kScaling);

  ENABLE_CAP(masking);
  ENABLE_CAP(accumulation);
  DISABLE_CAP(activation);
  WRITE_CAP(def_scaling, kScaling);

  WRITE_CAP(operations, 0);
  ENABLE_OPERATION(operations, static_cast<uint>(MatOperations::ADD));
  ENABLE_OPERATION(operations, static_cast<uint>(MatOperations::FMA));
  ENABLE_OPERATION(operations, static_cast<uint>(MatOperations::MUL));

  WRITE_CAP(num_cores, Q_PES);
  WRITE_CAP(datatype, static_cast<int>(Datatypes::FXP_NORM));
  WRITE_CAP(num_bits_integer, Q_INT);
  WRITE_CAP(num_bits_fraction, Q_BW - Q_INT);

  WRITE_CAP(accel_version, kAccelVersion);
  WRITE_CAP(accel_type, static_cast<int>(AcceleratorType::MATRIX));
}

/**
 * Defines the parameters checking function. This function checks if the
 * parameters are within the supported ranges. Here, the user can define
 * the operation ranges of the accelerator.
 * Params:
 * - rows
 * - columns
 * - load_matrix_mask
 * - en_accumulation
 * - en_activation
 * - scaling
 * - operation
 * - execution_mode
 * @param function name
 */
DEF_SET_MATRIX_PARAMS_CHECK(gemm_params_check) {
  /* Dimensions */
  CHECK_RANGE_AND_FALLBACK(rows, kRows, kMatrixMaxSize, kRows);
  CHECK_RANGE_AND_FALLBACK(columns, kColumns, kMatrixMaxSize, kColumns);

  CHECK_RANGE_AND_FALLBACK(
      load_matrix_mask, static_cast<int>(MatMasks::INVALID),
      static_cast<int>(MatMasks::ALL), static_cast<int>(MatMasks::ALL));

  CHECK_EQUALITY_AND_FALLBACK(scaling, kScaling, kScaling);

  WRITE_EXE_PARAM(en_accumulation, READ_EXE_PARAM(en_accumulation));
  CHECK_EQUALITY_AND_FALLBACK(en_activation, PropertyPort{0}, PropertyPort{0});

  WRITE_EXE_PARAM(execution_mode, READ_EXE_PARAM(execution_mode));
  WRITE_EXE_PARAM(operation, READ_EXE_PARAM(operation));
}

/**
 * Definition of the top function. This function defines the logic of the
 * accelerator. Please, keep the logic isolated from the top function in
 * separate files.
 *
 * Streaming ports:
 * - stream_input
 * - stream_output
 *
 * Capabilities available:
 * - rows
 * - columns
 * - scaling
 * - masking
 * - accumulation
 * - activation
 * - def_scaling
 * - operations
 * - num_cores
 * - datatype
 * - num_bits_integer
 * - num_bits_fraction
 * - accel_type
 * - accel_version
 *
 * Params:
 * - rows
 * - columns
 * - load_matrix_mask
 * - en_accumulation
 * - en_activation
 * - scaling
 * - operation
 * - execution_mode
 *
 * @param gemm_accel name of the top function
 * @param gemm_capabilities name of the capabilities function
 * @param gemm_params_check name of the params checking function
 */
BEGIN_DEF_TOP_MATRIX_FUNCTION(gemm_accel, gemm_capabilities, gemm_params_check)
accel_def : {
  /* Temporal buffers */
  static DataType A[kRows][kColumns] = {0.f};
  static DataType B[kRows][kColumns] = {0.f};
  static DataType C[kRows][kColumns] = {0.f};

  DimensionPort rows = READ_EXE_PARAM(rows);
  DimensionPort columns = READ_EXE_PARAM(columns);
  PropertyPort mask = READ_EXE_PARAM(load_matrix_mask);
  PropertyPort accumulate = READ_EXE_PARAM(en_accumulation);

  if (stream_input.empty()) return;

  /* Call the implementations */
  load_data(stream_input, A, B, C, mask, rows, columns);

  execute(A, B, C);

  retrieve_data(stream_output, C, accumulate, rows, columns);
}

END_DEF_TOP_MATRIX_FUNCTION()
