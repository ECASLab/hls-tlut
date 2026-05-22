/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example simple-consumer.cpp
 * Integration example.
 * Please, look at the integration folder for details. In your meson.build file,
 * integrate the dependency as:
 * @code
 * axc_math_proj = subproject('approximate-math-operators')
 * axc_math_dep = axc_math_proj.get_variable('axc_math_operators_dep')
 * @endcode
 * You may need to integrate the wrap file.
 * @ref ./subprojects/approximate-math-operators.wrap
 */

#include <iostream>
#include <operators.hpp>

int main(int, char **) {
  auto add_ = axc::arithmetic::exact::Add<int>{};
  std::cout << "2 + 2 = " << add_(2, 2) << std::endl;
  return 0;
}
