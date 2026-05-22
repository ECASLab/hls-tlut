/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include <ap_fixed.h>
#include <cmath>
#include <iostream>

#include <approximate/arithmetic.hpp>

int main(int, char **) {
  constexpr int kLSB = 2;
  using DataType = ap_fixed<10, 3>;

  axc::arithmetic::approximate::LsbOr<DataType, kLSB> op{};

  DataType op1 = 1.f;
  DataType op2 = 2.f;

  std::cout << op1 << " + " << op2 << " = " << op.Plus(op1, op2) << std::endl
            << op1 << " - " << op2 << " = " << op.Minus(op1, op2) << std::endl
            << op1 << " * " << op2 << " = " << op.Mult(op1, op2) << std::endl
            << op1 << " / " << op2 << " = " << op.Div(op1, op2) << std::endl;

  return 0;
}
