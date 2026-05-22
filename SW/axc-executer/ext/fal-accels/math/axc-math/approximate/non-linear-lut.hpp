/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include "exponential-lut.hpp"
#include "tanh-lut.hpp"

namespace axc {
namespace nonlinear {
namespace approximate {

/**
 * Class in charge of performing LUT-based functions with
 * domain-specific characteristics
 * @tparam T type of the LUT array
 * @tparam BEGIN begin of the function domain. Defaults to -1 (std::ratio)
 * @tparam END end of the function domain. Defaults to 1 (std::ratio)
 * @tparam S size of the LUT array. It defaults to 64 elements from BEGIN to
 * END
 */
template <typename T, class BEGIN, class END, int S = 64>
class LUTMath {
 public:
  /**
   * @brief Number of points of the look-up-table to initialise
   */
  static constexpr int Points = S;

  /**
   * @brief Beginning of the look-up-table to initialise
   */
  static constexpr float Minimum =
      static_cast<float>(BEGIN::num) / static_cast<float>(BEGIN::den);

  /**
   * @brief Ending of the look-up-table to initialise
   */
  static constexpr float Maximum =
      static_cast<float>(END::num) / static_cast<float>(END::den);

  /**
   * @brief Define the step
   */
  static constexpr float Step =
      (Maximum - Minimum) / static_cast<float>(Points);

  /**
   * @brief Performs the hyperbolic tangent based on LUT
   *
   * @param x input
   * @return T output tanh(x)
   */
  T TanH(const T x) {
#pragma HLS INLINE RECURSIVE
    return tanh_(x);
  }

  /**
   * @brief Performs the exponential function based on LUT
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
  lut::TanH<T, BEGIN, END, S> tanh_;
  /** Exponential function operator */
  lut::Exponential<T, BEGIN, END, S> exp_;
};

}  // namespace approximate
}  // namespace nonlinear
}  // namespace axc
