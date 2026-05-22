/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>

#include "convolution.hpp"

using DataType = ap_fixed<Q_BW, 1>;

static const int kOutputSize =
    ama::hw::convolvers::Convolver<DataType, Q_K, Q_O>::outputsize;
static const int kWindowSize =
    ama::hw::convolvers::Convolver<DataType, Q_K, Q_O>::windowsize;
static const int kKernelSize =
    ama::hw::convolvers::Convolver<DataType, Q_K, Q_O>::kernelsize;

void single_core_top_accel(DataType input[kWindowSize][kWindowSize],
                           DataType kernel[kKernelSize][kKernelSize],
                           DataType output[kOutputSize][kOutputSize]);
