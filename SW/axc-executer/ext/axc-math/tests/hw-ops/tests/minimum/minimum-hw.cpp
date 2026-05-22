/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "exact/multiplication.hpp"
#include "minimum.hpp"
/**
 * @example minimum.cpp
 * This is an example of a logic source
 */

/* You can use any HLS library here such as ap_fixed.h and ap_int.h */

/* You can also use any file in:
   convolution/include, matrix/includes, math/axc-math */

void multiply(const unsigned int a, const unsigned int b, unsigned int& c) {
  /* Performs a multiplication between unsigned integers */
  static auto multiplier = axc::arithmetic::exact::Multiply<decltype(a)>{};
  c = multiplier(a, b);
}
