/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <axc-math/exact/addition.hpp>
#include <axc-math/exact/multiplication.hpp>
#include <cmath>
#include <numeric>

namespace Kernels {
namespace Arithmetic {
/**
 * @brief Built-in arithmetic
 *
 * @tparam T type
 */
template <class T, int LA = 0, int LM = 0>
struct Exact {
  /**
   * @brief Plus or addition function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T plus(const T op1, const T op2) const { return op1 + op2; }

  /**
   * @brief Multiplication function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T mult(const T op1, const T op2) const { return op1 * op2; }
};

}  // namespace Arithmetic
}  // namespace Kernels
