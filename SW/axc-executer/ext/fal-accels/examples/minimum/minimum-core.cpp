/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "minimum.hpp"

/**
 * @example minimum-core.cpp
 * This is an example of a core file (top function)
 */

/* You can use any HLS library here such as ap_fixed.h and ap_int.h */

/* You can also use any wrapper in include/wrappers: i.e.
   #include "wrappers/matrix.hpp" */

/* You can also use any file in:
   convolution/include, matrix/includes, math/axc-math */

void minimum_accel_top(const unsigned int a, const unsigned int b,
                       unsigned int& c) {
  multiply(a, b, c);
}
