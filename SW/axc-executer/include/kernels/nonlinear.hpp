/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>

namespace Kernels {
namespace NonLinear {
/**
 * @brief STL-based activations
 *
 * @tparam T type
 */
template <class T>
struct STL {
  /**
   * @brief General exponential function
   * The domain is in IR
   *
   * @param op input operand
   * @return T result
   */
  T exp(const T op) const {
    float fop = op;
    float fres = std::exp(fop);
    T res = fres;
    return res;
  }

  /**
   * @brief General hyperbolic tangent function
   * The domain is in IR
   *
   * @param op input operand
   * @return T result
   */
  T tanh(const T op) const {
    float fop = op;
    float fres = std::tanh(fop);
    T res = fres;
    return res;
  }

  /**
   * @brief General ReLU function
   * The domain is in IR
   *
   * @param op input operand
   * @return T result
   */
  T relu(const T op) const { return op > T{0} ? op : T{0}; }

  /**
   * @brief General PassThru function
   * The domain is in IR
   *
   * @param op input operand
   * @return T result
   */
  T passthru(const T op) const { return op; }
};
}  // namespace NonLinear
}  // namespace Kernels
