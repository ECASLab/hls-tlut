/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include <ap_fixed.h>
#include <cmath>
#include <iostream>
#include <ratio>

#include <approximate/non-linear.hpp>

int main(int, char **) {
  constexpr int kNumPoints = 64;
  constexpr int kOrder = 3;
  using Start = std::ratio<-1>;
  using End = std::ratio<1>;
  using DataType = ap_fixed<10, 3>;

  /* Declare the operator - LUT-based */
  using TanHLutOp =
      axc::nonlinear::approximate::lut::TanH<DataType, Start, End, kNumPoints>;
  TanHLutOp tanh_lut_op{};

  /* Declare the operator - Exponential-based */
  using ExpLUTOp =
      axc::nonlinear::approximate::lut::Exponential<DataType, Start, End,
                                                    kNumPoints>;
  using ExpTaylorOp =
      axc::nonlinear::approximate::taylor::Exponential<DataType, kOrder>;
  using TanHExpLUTOp =
      axc::nonlinear::approximate::exp::TanH<DataType, ExpLUTOp>;
  using TanHExpTaylorOp =
      axc::nonlinear::approximate::exp::TanH<DataType, ExpTaylorOp>;
  TanHExpLUTOp tanh_exp_lut_op{};
  TanHExpTaylorOp tanh_exp_taylor_op{};

  /* Declare the operator - Taylor-based */
  using TanHTaylorOp =
      axc::nonlinear::approximate::taylor::TanH<DataType, kOrder>;
  TanHTaylorOp tanh_taylor{};

  /* Generate the differences */
  std::cout << std::setw(10) << "x" << std::setw(10) << "xa" << std::setw(10)
            << "exact" << std::setw(10) << "approxlut" << std::setw(10)
            << "approxexplut" << std::setw(10) << "approxexptaylor"
            << std::setw(10) << "approxtaylor" << std::endl;
  for (float x = TanHLutOp::Generator::Minimum;
       x < TanHLutOp::Generator::Maximum;
       x += TanHLutOp::Generator::Step * 0.5) {
    DataType xa = x;
    std::cout << std::setw(10) << x << std::setw(10) << xa << std::setw(10)
              << std::tanh(x) << std::setw(10) << tanh_lut_op(xa)
              << std::setw(10) << tanh_exp_lut_op(xa) << std::setw(10)
              << tanh_exp_taylor_op(xa) << std::setw(10) << tanh_taylor(xa)
              << std::endl;
  }

  return 0;
}
