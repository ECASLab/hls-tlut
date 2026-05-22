/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <iostream>

namespace ama {
namespace utils {

template <typename T, int K>

/**
 * @brief Prints a matrix
 * @tparam T datatype
 * @tparam K kernel size
 */

void add_kernel3(T kernel_output[K][K]) { 
  float kernel_base[3][3] = {
      {0.0625, 0.125, 0.0625}, {0.125, 0.25, 0.125}, {0.0625, 0.125, 0.0625}};
  for (int i{0}; i < K; ++i) {
    for (int j{0}; j < K; ++j) {
      kernel_output[i][j] = kernel_base[i][j];
    }
  }
}
}  // namespace utils
}  // namespace ama
