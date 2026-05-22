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
  constexpr int kOrder = 2;
  using Start = std::ratio<-1>;
  using End = std::ratio<1>;
  constexpr float start = Start::num / Start::den;
  constexpr float end = End::num / End::den;
  constexpr float step = 0.5f / kNumPoints;
  using DataType = ap_fixed<10, 3>;

  /* Declare the operator */
  using namespace axc::nonlinear::approximate::lut;
  using ExpOpLUT =
      axc::nonlinear::approximate::lut::Exponential<DataType, Start, End,
                                                    kNumPoints>;
  using ExpOpTaylor =
      axc::nonlinear::approximate::taylor::Exponential<DataType, kOrder>;
  ExpOpLUT explut{};
  ExpOpTaylor exptaylor{};

  /* Generate the differences */
  std::cout << std::setw(10) << "x" << std::setw(10) << "xa" << std::setw(10)
            << "exact" << std::setw(10) << "approx-lut" << std::setw(10)
            << "approx-taylor" << std::endl;
  for (float x = start; x < end; x += step) {
    DataType xa = x;
    std::cout << std::setw(10) << x << std::setw(10) << xa << std::setw(10)
              << std::exp(x) << std::setw(10) << explut(xa) << std::setw(10)
              << exptaylor(xa) << std::endl;
  }

  return 0;
}
