/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ap_fixed.h>

#include <runtime.hpp>

static constexpr int DataWidthBits = 24;
static constexpr int IntWidthBits = 8;

namespace Kernels {
namespace FAL {
/**
 * @brief FAL convolution implementation. It uses the accelerator implementation
 * in FAL
 *
 * It implements only the ap_fixed<24, 8> accelerator
 */
struct Conv2D_Spatial_24_8 {
  using Q = ap_fixed<DataWidthBits, IntWidthBits>;

  /** Number of samples included in a batch */
  int target_samples = 0;

  /** Number of channels included in an output sample */
  int target_channels = 0;

  /** Width of an output sample */
  int target_width = 0;

  /** Height of an output sample */
  int target_height = 0;

  /** Number of channels included in an input sample */
  int input_channels = 0;

  /** Width of an input sample */
  int input_width = 0;

  /** Height of an input sample */
  int input_height = 0;

  /** Width of a kernel */
  int k_width = 0;

  /** Height of a kernel */
  int k_height = 0;

  /** Stride X: Not supported! */
  int stride_x = 1;

  /** Stride Y: Not supported! */
  int stride_y = 1;

  /** Padding X: padding for each side (i.e. 1 -> total padding is 2) */
  int padding_x = 0;

  /** Padding Y: padding for each side (i.e. 1 -> total padding is 2) */
  int padding_y = 0;

  /**
   * @brief Functor operator()
   *
   * It is implemented in the CPP for isolation. This is where the logic of to
   * call the accelerator will take place. In this case, it is a Conv2D
   * accelerator with 24 bits of data width and 8 bits of integer part.
   *
   * @param k_data pointer to the kernels
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const Q *k_data, const Q *input_data, Q *output_data);
};
}  // namespace FAL
}  // namespace Kernels
