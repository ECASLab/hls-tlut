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

void add_kernel7(T kernel_output[K][K]) {
  float kernel_base[7][7] = {{0, 0, 0.000997008, 0.001994017, 0.000997008, 0, 0},
                   {0, 0.002991026, 0.01296111665, 0.02193419741, 0.01296111665,
                    0.002991026, 0},
                   {0.000997008, 0.01296111665, 0.05882352941, 0.09670987039,
                    0.05882352941, 0.01296111665, 0.000997008},
                   {0.001994017, 0.02193419741, 0.09670987039, 0.1585244267,
                    0.09670987039, 0.02193419741, 0.001994017},
                   {0.000997008, 0.01296111665, 0.05882352941, 0.09670987039,
                    0.05882352941, 0.01296111665, 0.000997008},
                   {0, 0.002991026, 0.01296111665, 0.02193419741, 0.01296111665,
                    0.002991026, 0},
                   {0, 0, 0.000997008, 0.001994017, 0.000997008, 0, 0}};
  for (int i{0}; i < K; ++i) {
    for (int j{0}; j < K; ++j) {
      kernel_output[i][j] = kernel_base[i][j];
    }
  }
}
}  // namespace utils
}  // namespace ama
