/*
 * Copyright 2022-2023
 * Author: Fabricio Elizondo Fernandez <faelizondo@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ratio>

namespace axc {
namespace nonlinear {
namespace approximate {
namespace exp {

/**
 * Class in charge of performing exp-based hyperbolic tangent function with
 * domain-specific characteristics
 * @tparam T type of the LUT array
 * @tparam Exp class that provides the exponential function
 */
template <typename T, class Exp>
class TanH {
 public:
  /**
   * @brief Construct a new Tan H object
   */
  TanH() : exp_{} {}

  /**
   * @brief Computes the tanh(x) of x
   *
   * @param x input
   * @return T output (tanh(x))
   */
  T operator()(const T x) {
#pragma HLS PIPELINE
    using InternalT = ap_fixed<2 * T::width, T::width>;

    InternalT exp_x = exp_(x);
    InternalT exp_neg_x = exp_(-x);

    InternalT den = 1 / (exp_x + exp_neg_x);
    InternalT num = (exp_x - exp_neg_x);

    return num * den;
  }

  /**
   * @brief Exponential factory
   */
  typedef Exp Generator;

 private:
  Generator exp_;
};

}  // namespace exp
}  // namespace approximate
}  // namespace nonlinear
}  // namespace axc
