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

void add_kernel5(T kernel_output[K][K]) {
  float kernel_base[5][5] = {
      {0.003663003, 0.01465201465, 0.02564102564, 0.01465201465, 0.003663003},
      {0.01465201465, 0.05860805861, 0.09523809524, 0.05860805861,
       0.01465201465},
      {0.02564102564, 0.09523809524, 0.1501831502, 0.09523809524,
       0.02564102564},
      {0.01465201465, 0.05860805861, 0.09523809524, 0.05860805861,
       0.01465201465},
      {0.003663003, 0.01465201465, 0.02564102564, 0.01465201465, 0.003663003}};
  for (int i{0}; i < K; ++i) {
    for (int j{0}; j < K; ++j) {
      kernel_output[i][j] = kernel_base[i][j];
    }
  }
}
}  // namespace utils
}  // namespace ama
