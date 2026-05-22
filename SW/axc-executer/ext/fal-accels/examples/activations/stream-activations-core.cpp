/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include "stream-activations.hpp"

#include "wrappers/matrix.hpp"

/* --- Accelerator routines --- */

/**
 * Define the activations capabilities of the accelerator. This function
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
DEF_SET_MATRIX_CAPABILITIES(activations_capabilities) {
  WRITE_RANGE_CAP(rows, kRows, kMatrixMaxSize);
  WRITE_RANGE_CAP(columns, kColumns, kMatrixMaxSize);
  WRITE_RANGE_CAP(scaling, kScaling, kScaling);

  DISABLE_CAP(masking);
  DISABLE_CAP(accumulation);
  ENABLE_CAP(activation);
  WRITE_CAP(def_scaling, kScaling);

  WRITE_CAP(operations, 0);
  ENABLE_OPERATION(operations, static_cast<uint>(MatOperations::TANH));

  WRITE_CAP(num_cores, Q_PES);
  WRITE_CAP(datatype, static_cast<int>(Datatypes::FXP_NORM));
  WRITE_CAP(num_bits_integer, Q_INT);
  WRITE_CAP(num_bits_fraction, Q_BW - Q_INT);

  WRITE_CAP(accel_version, kAccelVersion);
  WRITE_CAP(accel_type, static_cast<int>(AcceleratorType::MAP));
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
DEF_SET_MATRIX_PARAMS_CHECK(activations_params_check) {
  /* Dimensions */
  CHECK_RANGE_AND_FALLBACK(rows, kRows, kMatrixMaxSize, kRows);
  CHECK_RANGE_AND_FALLBACK(columns, kColumns, kMatrixMaxSize, kColumns);

  /* Not used: Pass-thru to avoid dangling ports */
  DECLARE_AS_NOT_USED(execution_mode);
  DECLARE_AS_NOT_USED(operation);
  DECLARE_AS_NOT_USED(en_accumulation);
  DECLARE_AS_NOT_USED(load_matrix_mask);
  DECLARE_AS_NOT_USED(en_activation);
  DECLARE_AS_NOT_USED(scaling);
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
 * @param activations_accel name of the top function
 * @param activations_capabilities name of the capabilities function
 * @param activations_params_check name of the params checking function
 */
BEGIN_DEF_TOP_MATRIX_FUNCTION(activations_accel, activations_capabilities,
                              activations_params_check)
accel_def : {
  /* Temporal buffers */
  static DataType A[kRows][kColumns] = {0.f};
  static DataType B[kRows][kColumns] = {0.f};

  DimensionPort rows = READ_EXE_PARAM(rows);
  DimensionPort columns = READ_EXE_PARAM(columns);

  if (stream_input.empty()) return;

  /* Call the implementations */
  load_data(stream_input, A, rows, columns);

  execute(A, B);

  retrieve_data(stream_output, B, rows, columns);
}

END_DEF_TOP_MATRIX_FUNCTION()
