/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

/* Approximate version */
#include "addition-lsbdrop.hpp"
#include "addition-lsbor.hpp"
#include "multiplication-lsbdrop.hpp"
#include "multiplication-lsbor.hpp"

namespace axc {
namespace arithmetic {
namespace approximate {

/**
 * @brief LSB Drop approximate arithmetic
 *
 * It performs the basic arithmetic operators using the LSB Drop approximation.
 * It is possible to tune the approximation level through the template
 * parameters of LA y LM.
 *
 * @tparam T data type
 * @tparam LA number of LSB bits to approximate in the adder
 * @tparam LM number of LSB bits to approximate in the multiply. It falls back
 * to LA in case of undefined
 */
template <class T, int LA = 1, int LM = 0>
class LsbDrop {
 public:
  /** Number of approximate bits of the multiplier */
  static constexpr int LossMult = LM <= 0 ? LA : LM;
  /** Number of approximate bits of the adder */
  static constexpr int LossAdd = LA;

  using MultOp =
      axc::arithmetic::approximate::lsbdrop::Multiply<T, T::width, T::iwidth,
                                                      LossMult>;
  using AddOp = axc::arithmetic::approximate::lsbdrop::Add<T, T::width,
                                                           T::iwidth, LossAdd>;

  /**
   * @brief Plus or addition function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Plus(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return addop_(op1, op2);
  }

  /**
   * @brief Multiplication function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Mult(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return multop_(op1, op2);
  }

  /**
   * @brief Minus or substraction function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Minus(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return addop_(op1, -op2);
  }

  /**
   * @brief Division function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Div(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return multop_(op1, T{1.f} / op2);
  }

 private:
  /** Adder operator */
  AddOp addop_;
  /** Multiplier operator */
  MultOp multop_;
};

/**
 * @brief LSB Drop approximate arithmetic
 *
 * It performs the basic arithmetic operators using the LSB OR approximation. It
 * is possible to tune the approximation level through the template parameters
 * of LA y LM.
 *
 * @tparam T data type
 * @tparam LA number of LSB bits to approximate in the adder
 * @tparam LM number of LSB bits to approximate in the multiply. It falls back
 * to LA in case of undefined
 */
template <class T, int LA = 1, int LM = 0>
class LsbOr {
 public:
  /** Number of approximate bits of the multiplier */
  static constexpr int LossMult = LM <= 0 ? LA : LM;
  /** Number of approximate bits of the adder */
  static constexpr int LossAdd = LA;
  using MultOp =
      axc::arithmetic::approximate::lsbor::Multiply<T, T::width, T::iwidth,
                                                    LossMult>;
  using AddOp =
      axc::arithmetic::approximate::lsbor::Add<T, T::width, T::iwidth, LossAdd>;

  /**
   * @brief Plus or addition function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Plus(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return addop_(op1, op2);
  }

  /**
   * @brief Multiplication function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Mult(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return multop_(op1, op2);
  }

  /**
   * @brief Minus or substraction function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Minus(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return addop_(op1, -op2);
  }

  /**
   * @brief Division function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Div(const T op1, const T op2) {
#pragma HLS INLINE RECURSIVE
    return multop_(op1, T{1.f} / op2);
  }

 private:
  /** Adder operator */
  AddOp addop_;
  /** Multiplier operator */
  MultOp multop_;
};

}  // namespace approximate
}  // namespace arithmetic
}  // namespace axc
