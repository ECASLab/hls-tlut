/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example stream-convolution-core.cpp
 * This is the core implementation of the convolution accelerator
 */

#include "stream-convolution.hpp"

/**
 * Define the convolution capabilities of the accelerator. This function
 * writes ranges of the operands, supported capabilities, accelerator type
 * and acceleration version.
 * Capabilities available:
 * - input_width
 * - input_height
 * - output_width
 * - output_height
 * - dilatation
 * - scaling
 * - num_inputs
 * - kernel_size
 * - datatype
 * - num_bits_integer
 * - num_bits_fraction
 * - accel_type
 * - accel_version
 * @param function name
 */
DEF_SET_CONV_CAPABILITIES(convolution_capabilities) {
  WRITE_RANGE_CAP(input_width, kWindowSize, kMaximumImageSize);
  WRITE_RANGE_CAP(input_height, kRowsWindow, kMaximumImageSize);
  WRITE_RANGE_CAP(output_width, kOutputSize, kMaximumImageSize);
  WRITE_RANGE_CAP(output_height, kRowsOutput, kMaximumImageSize);

  DISABLE_CAP(stride);
  DISABLE_CAP(dilatation);
  DISABLE_CAP(padding);
  DISABLE_CAP(bias);

  WRITE_CAP(num_inputs, 1);
  WRITE_RANGE_CAP(scaling, 1, 1);
  WRITE_CAP(num_kernels, 1);
  WRITE_CAP(kernel_size, kKernelSize);
  WRITE_CAP(num_cores, Q_PES);
  WRITE_CAP(datatype, static_cast<int>(Datatypes::FXP_NORM));
  WRITE_CAP(num_bits_integer, Q_INT);
  WRITE_CAP(num_bits_fraction, Q_BW - Q_INT);

  WRITE_CAP(accel_version, kAccelVersion);
  WRITE_CAP(accel_type, static_cast<int>(AcceleratorType::CONVOLUTION));
}

/**
 * Defines the parameters checking function. This function checks if the
 * parameters are within the supported ranges. Here, the user can define
 * the operation ranges of the accelerator.
 * Params:
 * - input_width
 * - input_height
 * - output_width
 * - output_height
 * - kernel_size
 * - num_kernels
 * - output_scaling
 * - padding_type
 * - stride_x
 * - stride_y
 * - dilatation_x
 * - dilatation_y
 * - execution_mode
 * @param function name
 */
DEF_SET_CONV_PARAMS_CHECK(convolution_params_check) {
  /* Dimensions */
  CHECK_RANGE_AND_FALLBACK(input_width, kWindowSize, kMaximumImageSize,
                           kWindowSize);
  CHECK_RANGE_AND_FALLBACK(output_width, kOutputSize, kMaximumImageSize,
                           kOutputSize);
  CHECK_RANGE_AND_FALLBACK(input_height, kWindowSize, kMaximumImageSize,
                           kWindowSize);
  CHECK_RANGE_AND_FALLBACK(output_height, kOutputSize, kMaximumImageSize,
                           kOutputSize);

  /* Runtime params */
  CHECK_EQUALITY_AND_FALLBACK(kernel_size, PropertyPort{Q_K},
                              PropertyPort{Q_K});
  CHECK_EQUALITY_AND_FALLBACK(num_kernels, PropertyPort{1}, PropertyPort{1});
  CHECK_EQUALITY_AND_FALLBACK(output_scaling, PropertyPort{1}, PropertyPort{1});

  const PropertyPort kDefPadding = static_cast<int>(PaddingType::NONE);
  CHECK_EQUALITY_AND_FALLBACK(padding_type, kDefPadding, kDefPadding);
  CHECK_EQUALITY_AND_FALLBACK(stride_x, PropertyPort{0}, PropertyPort{0});
  CHECK_EQUALITY_AND_FALLBACK(stride_y, PropertyPort{0}, PropertyPort{0});
  CHECK_EQUALITY_AND_FALLBACK(dilatation_x, PropertyPort{0}, PropertyPort{0});
  CHECK_EQUALITY_AND_FALLBACK(dilatation_y, PropertyPort{0}, PropertyPort{0});

  WRITE_EXE_PARAM(execution_mode, READ_EXE_PARAM(execution_mode));
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
 * Capabilities:
 * - input_width
 * - input_height
 * - output_width
 * - output_height
 * - dilatation
 * - scaling
 * - num_inputs
 * - kernel_size
 * - datatype
 * - num_bits_integer
 * - num_bits_fraction
 * - accel_type
 * - accel_version
 *
 * Params:
 * - input_width
 * - input_height
 * - output_width
 * - output_height
 * - kernel_size
 * - num_kernels
 * - output_scaling
 * - padding_type
 * - stride_x
 * - stride_y
 * - dilatation_x
 * - dilatation_y
 * - execution_mode
 *
 * @param convolution_accel name of the top function
 * @param convolution_capabilities name of the capabilities function
 * @param convolution_params_check name of the params checking function
 */
BEGIN_DEF_TOP_CONV_FUNCTION(convolution_accel, convolution_capabilities,
                            convolution_params_check) 
accel_def: {
  /* Temporal buffers */
  static DataType input_img[kRowsWindow][kWindowSize];
  static DataType output_img[kRowsOutputPad][kOutputSize];
  static DataType kernel_img[kKernelSize][kKernelSize];
#pragma HLS ARRAY_PARTITION variable = input_img complete dim = 0
#pragma HLS ARRAY_PARTITION variable = kernel_img complete dim = 0
#pragma HLS ARRAY_PARTITION variable = output_img complete dim = 0

  uint input_width = READ_EXE_PARAM(input_width);
  uint input_height = READ_EXE_PARAM(input_height);
  uint output_width = READ_EXE_PARAM(output_width);
  uint output_height = READ_EXE_PARAM(output_height);

  if (stream_input.empty()) return;

  /* Call the implementations */
  load_kernel(kernel_img, kernels);
  load_data(stream_input, input_img, input_width, input_height);
  execute(input_img, kernel_img, output_img);
  retrieve_data(stream_output, output_img, output_width, output_height);
}

END_DEF_TOP_CONV_FUNCTION()
