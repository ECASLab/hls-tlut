/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include "dataflow-addition.hpp"

#include "wrappers/streaming-binary.hpp"

/* --- Accelerator routines --- */

/**
 * Define the activations capabilities of the accelerator. This function
 * writes ranges of the operands, supported capabilities, accelerator type
 * and acceleration version.
 * Capabilities available:
 * - elements
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
DEF_SET_STREAM_BINARY_CAPABILITIES(addition_dataflow_capabilities) {
  WRITE_RANGE_CAP(elements, kMinElements, kMaxElements);
  WRITE_RANGE_CAP(scaling, kScaling, kScaling);

  DISABLE_CAP(masking);
  DISABLE_CAP(accumulation);
  DISABLE_CAP(activation);
  WRITE_CAP(def_scaling, kScaling);

  WRITE_CAP(operations, 0);
  ENABLE_OPERATION(operations, static_cast<uint>(MatOperations::ADD));

  WRITE_CAP(num_cores, kMinElements);
  WRITE_CAP(datatype, static_cast<int>(Datatypes::FXP_NORM));
  WRITE_CAP(num_bits_integer, Q_INT);
  WRITE_CAP(num_bits_fraction, Q_BW - Q_INT);

  WRITE_CAP(accel_version, kAccelVersion);
  WRITE_CAP(accel_type, static_cast<int>(AcceleratorType::DF_BINARY));
}

/**
 * Defines the parameters checking function. This function checks if the
 * parameters are within the supported ranges. Here, the user can define
 * the operation ranges of the accelerator.
 * Params:
 * - elements
 * - load_streaming_mask
 * - en_accumulation
 * - en_activation
 * - scaling
 * - operation
 * - execution_mode
 * @param function name
 */
DEF_SET_STREAM_BINARY_PARAMS_CHECK(addition_dataflow_params_check) {
  /* Dimensions */
  CHECK_RANGE_AND_FALLBACK(elements, kMinElements, kMaxElements, kMinElements);

  /* Not used: Pass-thru to avoid dangling ports */
  DECLARE_AS_NOT_USED(execution_mode);
  DECLARE_AS_NOT_USED(operation);
  DECLARE_AS_NOT_USED(en_accumulation);
  DECLARE_AS_NOT_USED(load_streaming_mask);
  DECLARE_AS_NOT_USED(en_activation);
  DECLARE_AS_NOT_USED(scaling);
}

/**
 * Definition of the top function. This function defines the logic of the
 * accelerator. Please, keep the logic isolated from the top function in
 * separate files.
 *
 * Streaming ports:
 * - stream_input_a
 * - stream_input_b
 * - stream_output
 *
 * Capabilities available:
 * - elements
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
 * - elements
 * - load_streaming_mask
 * - en_accumulation
 * - en_activation
 * - scaling
 * - operation
 * - execution_mode
 *
 * @param addition_dataflow_accel name of the top function
 * @param addition_dataflow_capabilities name of the capabilities function
 * @param addition_dataflow_params_check name of the params checking function
 */
BEGIN_DEF_TOP_STREAM_BINARY_FUNCTION(addition_dataflow_accel,
                                     addition_dataflow_capabilities,
                                     addition_dataflow_params_check)
accel_def : {
  /* Temporal buffers */
  static DataType A[1][kMinElements] = {0.f};
  static DataType B[1][kMinElements] = {0.f};
  static DataType C[1][kMinElements] = {0.f};

  DimensionPort elements = READ_EXE_PARAM(elements);

  if ((STREAM_BINARY_INPUT_A).empty()) return;
  if ((STREAM_BINARY_INPUT_B).empty()) return;

  /* Call the implementations */
addition_dataflow_accel_loop:
  for (DimensionPort i = elements; i > DimensionPort{0}; i -= kMinElements) {
    /* Data load */
    load_data(STREAM_BINARY_INPUT_A, A, 1, kMinElements);
    load_data(STREAM_BINARY_INPUT_B, B, 1, kMinElements);

    execute(A, B, C);

    /* Data execute */
    retrieve_data(STREAM_BINARY_OUTPUT, C, 1, kMinElements);
  }
}

END_DEF_TOP_STREAM_BINARY_FUNCTION()
