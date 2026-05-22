/*
 * Copyright 2022-2023
 * Author: Fabricio Elizondo Fernandez <faelizondo@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ap_fixed.h>

namespace axc {
namespace nonlinear {
namespace approximate {
namespace taylor {

/**
 * Class in charge of performing Taylor-based hyperbolic tangent function.
 *
 * It supports up to order seven.
 *
 * By definition, it is centred at zero. So, this approximation will outperform
 * on domains from -1 to 1
 * @tparam T data type
 * @tparam O order of the Taylor expansion
 */
template <typename T, int O = 1>
class TanH {
  using InternalT = ap_fixed<2 * T::width, T::width>;

 public:
  /**
   * @brief Order of the Taylor expansion
   */
  static constexpr int Order = O;

  /**
   * @brief Computes the tanh(x) of x
   *
   * @param x input
   * @return T output (tanh(x))
   */
  T operator()(const T x) {
    /* The limit is O <= 7 */
    static_assert(O <= 7 && O > 0,
                  "Taylor for tanh(x) only admits orders from 1 to 7");

    static const InternalT unit = InternalT{1.f};
    InternalT sum = 0;
    InternalT res = 0;
    InternalT num = unit;
    InternalT den = unit;

#pragma HLS pipeline
    for (int i = 0; i < O; ++i) {
      num *= x;
      res = kCoeff[i] * num;
      sum += res;
    }

    return sum;
  }

 private:
  static const InternalT kCoeff[7];
};

template <typename T, int O>
const typename TanH<T, O>::InternalT TanH<T, O>::kCoeff[] = {
    1.f, 0.f, -1.f / 3.f, 0.f, 2.f / 15.f, 0.f, -17.f / 315.f};

}  // namespace taylor
}  // namespace approximate
}  // namespace nonlinear
}  // namespace axc
