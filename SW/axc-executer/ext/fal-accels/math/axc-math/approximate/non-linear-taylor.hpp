/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include "exponential-taylor.hpp"
#include "tanh-taylor.hpp"

namespace axc {
namespace nonlinear {
namespace approximate {

/**
 * Class in charge of performing Taylor-based functions
 *
 * By definition, it is centred at zero. So, this approximation will outperform
 * on domains from -1 to 1
 * @tparam T data type
 * @tparam O order of the Taylor expansion
 */
template <typename T, int O = 1>
class TaylorMath {
 public:
  /**
   * @brief Order of the Taylor expansion
   */
  static constexpr int Order = O;

  /**
   * @brief Performs the hyperbolic tangent based on Taylor
   *
   * @param x input
   * @return T output tanh(x)
   */
  T TanH(const T x) {
#pragma HLS INLINE RECURSIVE
    return tanh_(x);
  }

  /**
   * @brief Performs the exponential function based on Taylor
   *
   * @param x input
   * @return T output exp(x)
   */
  T Exponential(const T x) {
#pragma HLS INLINE RECURSIVE
    return exp_(x);
  }

 private:
  /** Hyperbolic tangent operator */
  taylor::TanH<T, O> tanh_;
  /** Exponential function operator */
  taylor::Exponential<T, O> exp_;
};

}  // namespace approximate
}  // namespace nonlinear
}  // namespace axc
