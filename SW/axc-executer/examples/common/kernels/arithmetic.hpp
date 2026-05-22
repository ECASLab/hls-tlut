/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <axc-math/approximate/addition-lsbdrop.hpp>
#include <axc-math/approximate/addition-lsbfixed.hpp>
#include <axc-math/approximate/addition-lsbor.hpp>
#include <axc-math/approximate/multiplication-lsbdrop.hpp>
#include <axc-math/approximate/multiplication-lsbfixed.hpp>
#include <axc-math/approximate/multiplication-lsbor.hpp>
#include <cmath>
#include <numeric>

namespace Kernels {
namespace Arithmetic {
/**
 * @brief LSB Drop approximate arithmetic
 *
 * @tparam T type
 * @tparam LA number of LSB bits to approximate in the adder
 * @tparam LM number of LSB bits to approximate in the multiplier
 */
template <class T, int LA = 1, int LM = -1>
struct ApproximateLsbDrop {
  static constexpr int LossMult = LM <= 0 ? LA : LM;
  static constexpr int LossAdd = LA;
  static constexpr auto Mult =
      axc::arithmetic::approximate::lsbdrop::Multiply<T, T::width, T::iwidth,
                                                      LossMult>{};
  static constexpr auto Add =
      axc::arithmetic::approximate::lsbdrop::Add<T, T::width, T::iwidth,
                                                 LossAdd>{};

  /**
   * @brief Plus or addition function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T plus(const T op1, const T op2) const { return Add(op1, op2); }

  /**
   * @brief Multiplication function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T mult(const T op1, const T op2) const { return Mult(op1, op2); }
};

/**
 * @brief LSB Drop approximate arithmetic
 *
 * @tparam T type
 * @tparam LA number of LSB bits to approximate in the adder
 * @tparam LM number of LSB bits to approximate in the multiplier
 */
template <class T, int LA = 1, int LM = 0>
struct ApproximateLsbOr {
  static constexpr int LossMult = LM <= 0 ? LA : LM;
  static constexpr int LossAdd = LA;
  static constexpr auto Mult =
      axc::arithmetic::approximate::lsbor::Multiply<T, T::width, T::iwidth,
                                                    LossMult>{};
  static constexpr auto Add =
      axc::arithmetic::approximate::lsbor::Add<T, T::width, T::iwidth,
                                               LossAdd>{};

  /**
   * @brief Plus or addition function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T plus(const T op1, const T op2) const { return Add(op1, op2); }

  /**
   * @brief Multiplication function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T mult(const T op1, const T op2) const { return Mult(op1, op2); }
};

/**
 * @brief LSB Fixed approximate arithmetic
 *
 * @tparam T type
 * @tparam LA number of LSB bits to approximate in the adder
 * @tparam LM number of LSB bits to approximate in the multiplier
 */
template <class T, int LA = 1, int LM = 0>
struct ApproximateLsbFixed {
  static constexpr int LossMult = LM <= 0 ? LA : LM;
  static constexpr int LossAdd = LA;
  static constexpr auto Mult =
      axc::arithmetic::approximate::lsbfixed::Multiply<T, T::width, T::iwidth,
                                                       LossMult>{};
  static constexpr auto Add =
      axc::arithmetic::approximate::lsbfixed::Add<T, T::width, T::iwidth,
                                                  LossAdd>{};

  /**
   * @brief Plus or addition function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T plus(const T op1, const T op2) const { return Add(op1, op2); }

  /**
   * @brief Multiplication function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T mult(const T op1, const T op2) const { return Mult(op1, op2); }
};
}  // namespace Arithmetic
}  // namespace Kernels
