/*
 * Copyright 2022-2023
 * Author: Fabricio Elizondo Fernandez <faelizondo@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ratio>

#include "interpolation-wrapper.hpp"

namespace axc {
namespace nonlinear {
namespace approximate {
namespace lut {
namespace generator {

/**
 * Class in charge of initialising the LUT with the tanh values
 * @tparam T type of the LUT array
 * @tparam BEGIN begin of the function domain. Based on std::ratio
 * @tparam END end of the function domain. Based on std::ratio
 * @tparam S size of the LUT array. It defaults to 64 elements from BEGIN to
 * END
 */
template <typename T, class BEGIN, class END, int S = 64>
class TanH {
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
   * @brief Construct a new TanH LUT Generator object
   *
   * It also fills the LUT in the lut parameter with a domain-specific
   * tanh(x) function
   *
   * @param lut LUT array to fill
   */
  explicit TanH(T lut[S]);
};

template <typename T, class BEGIN, class END, int S>
TanH<T, BEGIN, END, S>::TanH(T lut[S]) {
#pragma HLS INLINE off
  for (int i = 0; i < Points; ++i) {
    float x = Minimum + i * Step;
    lut[i] = T{std::tanh(x)};
  }
}

}  // namespace generator

/**
 * Class in charge of performing LUT-based hyperbolic tangent function with
 * domain-specific characteristics
 * @tparam T type of the LUT array
 * @tparam BEGIN begin of the function domain. Defaults to -1
 * @tparam END end of the function domain. Defaults to 1
 * @tparam S size of the LUT array. It defaults to 64 elements from BEGIN to
 * END
 */
template <typename T, class BEGIN, class END, int S = 64>
class TanH {
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
   * @brief Computes the tanh(x) of x
   *
   * @param x input
   * @return T output (tanh(x))
   */
  T operator()(const T x) { return interpolator_(x); }

  /**
   * @brief LUT factory
   */
  typedef generator::TanH<T, BEGIN, END, S> Generator;

 private:
  ::axc::nonlinear::approximate::helpers::LinearInterpolation<T, Generator>
      interpolator_;
};

}  // namespace lut
}  // namespace approximate
}  // namespace nonlinear
}  // namespace axc
