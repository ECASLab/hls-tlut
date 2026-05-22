/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include <cmath>

#ifdef WITH_VIVADO_HLS
#include <hls_math.h>
#endif

namespace axc {
namespace nonlinear {
namespace exact {

/**
 * Class in charge of performing non-linear functions with
 * domain-specific characteristics
 * @tparam T data type
 */
template <typename T>
class Exact {
 public:
  /**
   * @brief Performs the hyperbolic tangent based on STL
   *
   * @param x input
   * @return T output tanh(x)
   */
  T TanH(const T x) {
#ifndef WITH_VIVADO_HLS
    float xf = x;
    return std::tanh(xf);
#else
    half xf = x;
    return hls::tanh(xf);
#endif
  }

  /**
   * @brief Performs the exponential function based on STL
   *
   * @param x input
   * @return T output exp(x)
   */
  T Exponential(const T x) {
#ifndef WITH_VIVADO_HLS
    float xf = x;
    return std::exp(xf);
#else
    half xf = x;
    return hls::exp(xf);
#endif
  }
};

}  // namespace exact
}  // namespace nonlinear
}  // namespace axc
