/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>
#include <hls_stream.h>

#include "accel_conv.hpp"
#include "accel_utils.hpp"
#include "convolution.hpp"

void accel_stream_top_accel(
    /* Streaming port */
    StreamPort& stream_input, StreamPort& stream_output,
    /* Execution params */
    DECL_EXE_PARAM(DimensionPort, input_width),
    DECL_EXE_PARAM(DimensionPort, input_height),
    DECL_EXE_PARAM(DimensionPort, output_width),
    DECL_EXE_PARAM(DimensionPort, output_height),
    DECL_EXE_PARAM(PropertyPort, kernel_size),
    DECL_EXE_PARAM(PropertyPort, num_kernels),
    DECL_EXE_PARAM(FullType, output_scaling),
    DECL_EXE_PARAM(PropertyPort, padding_type),
    DECL_EXE_PARAM(PropertyPort, stride_x),
    DECL_EXE_PARAM(PropertyPort, stride_y),
    DECL_EXE_PARAM(PropertyPort, dilatation_x),
    DECL_EXE_PARAM(PropertyPort, dilatation_y),
    DECL_EXE_PARAM(PropertyPort, execution_mode),
    /* Capabilities */
    DECL_RANGE_CAP(DimensionPort, input_width),
    DECL_RANGE_CAP(DimensionPort, input_height),
    DECL_RANGE_CAP(DimensionPort, output_width),
    DECL_RANGE_CAP(DimensionPort, output_height),
    DECL_CAP(PropertyPort, stride), DECL_CAP(PropertyPort, dilatation),
    DECL_CAP(PropertyPort, padding), DECL_RANGE_CAP(FullType, scaling),
    DECL_CAP(PropertyPort, bias), DECL_CAP(PropertyPort, num_inputs),
    DECL_CAP(PropertyPort, num_kernels), DECL_CAP(PropertyPort, kernel_size),
    DECL_CAP(PropertyPort, num_cores), DECL_CAP(PropertyPort, datatype),
    DECL_CAP(PropertyPort, num_bits_integer),
    DECL_CAP(PropertyPort, num_bits_fraction),
    /* Kernels */
    DataType kernels[kKernelSize * kKernelSize]);
