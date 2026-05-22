/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include <ap_fixed.h>
#include <iostream>

#include <exact/non-linear.hpp>

int main(int, char **) {
  using DataType = ap_fixed<10, 3>;

  axc::nonlinear::exact::Exact<DataType> op;

  DataType op1 = 0.5f;

  std::cout << "exp(" << op1 << ") = " << op.Exponential(op1) << std::endl
            << "tanh(" << op1 << ") = " << op.TanH(op1) << std::endl;

  return 0;
}
