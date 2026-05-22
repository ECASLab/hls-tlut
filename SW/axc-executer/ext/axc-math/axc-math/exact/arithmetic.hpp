/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

/* Approximate version */
#include "addition.hpp"
#include "division.hpp"
#include "multiplication.hpp"
#include "substraction.hpp"

namespace axc {
namespace arithmetic {
namespace exact {

/**
 * @brief Exact arithmetic
 *
 * It performs the basic arithmetic operators using the default exact operators.
 *
 * @tparam T data type
 * @tparam LA do nothing. Preserved because compatibility
 * @tparam LM do nothing. Preserved because compatibility
 */
template <class T, int LA = 0, int LM = 0>
class Exact {
 public:
  using MultOp = axc::arithmetic::exact::Multiply<T>;
  using AddOp = axc::arithmetic::exact::Add<T>;
  using DivOp = axc::arithmetic::exact::Divide<T>;
  using SubOp = axc::arithmetic::exact::Substract<T>;

  /**
   * @brief Plus or addition function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Plus(const T op1, const T op2) { return addop_(op1, op2); }

  /**
   * @brief Multiplication function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Mult(const T op1, const T op2) { return multop_(op1, op2); }

  /**
   * @brief Minus or substraction function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Minus(const T op1, const T op2) { return subop_(op1, op2); }

  /**
   * @brief Division function
   *
   * @param op1 first input operand
   * @param op2 second input operand
   * @return T result
   */
  T Div(const T op1, const T op2) { return divop_(op1, op2); }

 private:
  /** Adder operator */
  AddOp addop_;
  /** Multiplier operator */
  MultOp multop_;
  /** Divide operator */
  DivOp divop_;
  /** Substruct operator */
  SubOp subop_;
};

}  // namespace exact
}  // namespace arithmetic
}  // namespace axc
