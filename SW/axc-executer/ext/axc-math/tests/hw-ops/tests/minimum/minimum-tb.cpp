/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "minimum.hpp"

/**
 * @example minimum-tb.cpp
 * This is an example of a testbench source
 */

/* You can use any HLS library here such as ap_fixed.h and ap_int.h */

#include <iostream>

int main(int, char **) {
  unsigned int a{6}, b{3}, c{0};

  minimum_accel_top(a, b, c);
  std::cout << a << " * " << b << " = " << c << std::endl;
  return 0;
}
