/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/* Hardware ops */
#ifndef SW_ONLY
#include "hw-ops/Convolution.hpp"
#include "hw-ops/cores/arithmetic-exact.hpp"
#include "hw-ops/FFT.hpp"
#include "hw-ops/Padding.hpp"
#include "hw-ops/Space.hpp"
#include "hw-ops/Winograd.hpp"
#else
/* Software ops */
#include "sw-ops/Convolution.hpp"
#include "sw-ops/FFT_conv_2D.hpp"
#include "sw-ops/Padding.hpp"
#endif

/* Utils */
#include "utils/printmatrix.hpp"
#include "utils/Kernel3.hpp"
#include "utils/Kernel5.hpp"
#include "utils/Kernel7.hpp"
