/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

namespace ama {
namespace hw {

template <typename T, int M, int N, int k>

/**
 * @brief Matrix spatial convolution
 * It performs the convolution of two matrix.
 * @tparam T datatype
 * @tparam M the number of rows
 * @tparam N the number of columns
 * @tparam K the size of the kernel
 * @param kernel Matrix use as kernel
 * @param input_image Matrix padd for a kernel size k
 * @param output_image Matrix with the result of the convolution
 */

void conv(const T kernel[k][k], const T input_image[M + (k - 1)][N + (k - 1)],
          T output_image[M + (k - 1)][N + (k - 1)]) {
  const int start = k / 2;
  T sum = 0;
  for (int i{start}; i < M + start; ++i) {
    for (int j{start}; j < N + start; ++j) {
      for (int e{0}; e < k; ++e) {
        for (int r{0}; r < k; ++r) {
          sum += kernel[e][r] * input_image[i - start + e][j - start + r];
        }
      }
      output_image[i][j] = sum;
      sum = 0;
    }
  }
}
}  // namespace hw
}  // namespace ama
