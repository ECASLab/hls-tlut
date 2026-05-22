/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>

#define Q_BW 8
#define Q_K 3
using DataType = ap_fixed<Q_BW, 1>;

void space_top_accel(DataType input[Q_K + 1][Q_K + 1],
                     DataType kernel[Q_K][Q_K],
                     DataType output[Q_K - 1][Q_K - 1]);
