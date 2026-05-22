/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/**
 * @example stream-convolution-tb.hpp
 * This is the main testbench header of the convolution accelerator
 */

#include "stream-convolution.hpp"

/**
 * Macro for executing the accel with a more comfortable line
 * <b>Example:</b>
 * @code
 * EXECUTE_ACCEL;
 * @endcode
 */
#define EXECUTE_ACCEL                                                          \
  TOP_FUNCTION_NAME(                                                           \
      stream_input, stream_output, ACCESS_EXE_PARAM(input_width),              \
      ACCESS_EXE_PARAM(input_height), ACCESS_EXE_PARAM(output_width),          \
      ACCESS_EXE_PARAM(output_height), ACCESS_EXE_PARAM(kernel_size),          \
      ACCESS_EXE_PARAM(num_kernels), ACCESS_EXE_PARAM(output_scaling),         \
      ACCESS_EXE_PARAM(padding_type), ACCESS_EXE_PARAM(stride_x),              \
      ACCESS_EXE_PARAM(stride_y), ACCESS_EXE_PARAM(dilatation_x),              \
      ACCESS_EXE_PARAM(dilatation_y), ACCESS_EXE_PARAM(execution_mode),        \
      ACCESS_CAP_RANGE(input_width), ACCESS_CAP_RANGE(input_height),           \
      ACCESS_CAP_RANGE(output_width), ACCESS_CAP_RANGE(output_height),         \
      ACCESS_CAP(stride), ACCESS_CAP(dilatation), ACCESS_CAP(padding),         \
      ACCESS_CAP_RANGE(scaling), ACCESS_CAP(bias), ACCESS_CAP(num_inputs),     \
      ACCESS_CAP(num_kernels), ACCESS_CAP(kernel_size), ACCESS_CAP(num_cores), \
      ACCESS_CAP(datatype), ACCESS_CAP(num_bits_integer),                      \
      ACCESS_CAP(num_bits_fraction), ACCESS_CAP(accel_type),                   \
      ACCESS_CAP(accel_version), kernels);

/**
 * Macro for reading and printing the properties
 * <b>Example:</b>
 * @code
 * READ_PROPS;
 * @endcode
 */
#define READ_PROPS                                                           \
  std::cout << "\nExecution params"                                          \
            << "\n Input width: " << READ_EXE_PARAM_TB(input_width)          \
            << "\n Input height: " << READ_EXE_PARAM_TB(input_height)        \
            << "\n Output width: " << READ_EXE_PARAM_TB(output_width)        \
            << "\n Output height: " << READ_EXE_PARAM_TB(output_height)      \
            << "\n Kernel size: " << READ_EXE_PARAM_TB(kernel_size)          \
            << "\n Num Kernels: " << READ_EXE_PARAM_TB(num_kernels)          \
            << "\n Output Scaling: " << READ_EXE_PARAM_TB(output_scaling)    \
            << "\n Padding type: " << READ_EXE_PARAM_TB(padding_type)        \
            << "\n Stride X: " << READ_EXE_PARAM_TB(stride_x)                \
            << "\n Stride Y: " << READ_EXE_PARAM_TB(stride_y)                \
            << "\n Dilatation X: " << READ_EXE_PARAM_TB(dilatation_x)        \
            << "\n Dilatation Y: " << READ_EXE_PARAM_TB(dilatation_y)        \
            << "\n Execution mode: " << READ_EXE_PARAM_TB(execution_mode)    \
            << "\nCapabilities"                                              \
            << "\n Min input width: " << ACCESS_CAP_MIN(input_width)         \
            << "\n Max input width: " << ACCESS_CAP_MAX(input_width)         \
            << "\n Min input height: " << ACCESS_CAP_MIN(input_height)       \
            << "\n Max input height: " << ACCESS_CAP_MAX(input_height)       \
            << "\n Min output width: " << ACCESS_CAP_MIN(output_width)       \
            << "\n Max output width: " << ACCESS_CAP_MAX(output_width)       \
            << "\n Min output height: " << ACCESS_CAP_MIN(output_height)     \
            << "\n Max output height: " << ACCESS_CAP_MAX(output_height)     \
            << "\n Stride: " << ACCESS_CAP(stride)                           \
            << "\n Dilatation: " << ACCESS_CAP(dilatation)                   \
            << "\n Padding: " << ACCESS_CAP(padding)                         \
            << "\n Scaling Min: " << ACCESS_CAP_MIN(scaling)                 \
            << "\n Scaling Max: " << ACCESS_CAP_MAX(scaling)                 \
            << "\n Bias: " << ACCESS_CAP(bias)                               \
            << "\n Num inputs: " << ACCESS_CAP(num_inputs)                   \
            << "\n Num kernels: " << ACCESS_CAP(num_kernels)                 \
            << "\n Kernel size: " << ACCESS_CAP(kernel_size)                 \
            << "\n Num cores: " << ACCESS_CAP(num_cores)                     \
            << "\n Datatype: " << ACCESS_CAP(datatype)                       \
            << "\n Datatype - Integer: " << ACCESS_CAP(num_bits_integer)     \
            << "\n Datatype - Fractional: " << ACCESS_CAP(num_bits_fraction) \
            << "\n Accel type: " << ACCESS_CAP(accel_type)                   \
            << "\n Accel version: " << ACCESS_CAP(accel_version) << std::endl;

/**
 * Macro for declaring the testbench variables of the ports (execution
 * params)
 * <b>Example:</b>
 * @code
 * DECL_EXE_PARAMS_TB;
 * @endcode
 */
#define DECL_EXE_PARAMS_TB                         \
  DECL_EXE_PARAM_TB(DimensionPort, input_width);   \
  DECL_EXE_PARAM_TB(DimensionPort, input_height);  \
  DECL_EXE_PARAM_TB(DimensionPort, output_width);  \
  DECL_EXE_PARAM_TB(DimensionPort, output_height); \
  DECL_EXE_PARAM_TB(PropertyPort, kernel_size);    \
  DECL_EXE_PARAM_TB(PropertyPort, num_kernels);    \
  DECL_EXE_PARAM_TB(FullType, output_scaling);     \
  DECL_EXE_PARAM_TB(PropertyPort, padding_type);   \
  DECL_EXE_PARAM_TB(PropertyPort, stride_x);       \
  DECL_EXE_PARAM_TB(PropertyPort, stride_y);       \
  DECL_EXE_PARAM_TB(PropertyPort, dilatation_x);   \
  DECL_EXE_PARAM_TB(PropertyPort, dilatation_y);   \
  DECL_EXE_PARAM_TB(PropertyPort, execution_mode);

/**
 * Macro for declaring the testbench variables of the ports (capabilities)
 * <b>Example:</b>
 * @code
 * DECL_CAPS_TB;
 * @endcode
 */
#define DECL_CAPS_TB                               \
  DECL_RANGE_CAP_TB(DimensionPort, input_width);   \
  DECL_RANGE_CAP_TB(DimensionPort, input_height);  \
  DECL_RANGE_CAP_TB(DimensionPort, output_width);  \
  DECL_RANGE_CAP_TB(DimensionPort, output_height); \
  DECL_CAP_TB(PropertyPort, stride);               \
  DECL_CAP_TB(PropertyPort, dilatation);           \
  DECL_CAP_TB(PropertyPort, padding);              \
  DECL_RANGE_CAP_TB(FullType, scaling);            \
  DECL_CAP_TB(PropertyPort, bias);                 \
  DECL_CAP_TB(PropertyPort, num_inputs);           \
  DECL_CAP_TB(PropertyPort, num_kernels);          \
  DECL_CAP_TB(PropertyPort, kernel_size);          \
  DECL_CAP_TB(PropertyPort, num_cores);            \
  DECL_CAP_TB(PropertyPort, datatype);             \
  DECL_CAP_TB(PropertyPort, num_bits_integer);     \
  DECL_CAP_TB(PropertyPort, accel_type);           \
  DECL_CAP_TB(PropertyPort, accel_version);        \
  DECL_CAP_TB(PropertyPort, num_bits_fraction);
