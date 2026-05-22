/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include <ap_fixed.h>
#include <iostream>

#include <approximate/non-linear-taylor.hpp>

int main(int, char **) {
  constexpr int kOrder = 3;
  using DataType = ap_fixed<10, 3>;

  axc::nonlinear::approximate::TaylorMath<DataType, kOrder> op;

  DataType op1 = 0.5f;

  std::cout << "exp(" << op1 << ") = " << op.Exponential(op1) << std::endl
            << "tanh(" << op1 << ") = " << op.TanH(op1) << std::endl;

  return 0;
}
