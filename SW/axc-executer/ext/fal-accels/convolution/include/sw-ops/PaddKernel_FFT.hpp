/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once
#include "../utils/FFT_header.hpp"

namespace ama {
namespace sw {

/**
 * @brief Matrix padding
 * It performs the padding on a matrix for a kernel size k.
 * @param input_kernel Matrix original kernel
 * @param output_image kernel with the padding applied
 */

template <typename T, int M, int N, int k>

void paddkernel_FFT(const T input_kernel[k][k], Complex<T> output_image[M][N]) {
  int start[2] = {0};
  const int offset = (k + 1) / 2;

  start[0] = (M / 2) - offset;
  start[1] = (N / 2) - offset;


  for (int i{0}; i < k; ++i) {
    for (int j{0}; j < k; ++j) {
      output_image[start[0] + i][start[1] + j] = input_kernel[i][j];
    }
  }
}
}  // namespace sw
}  // namespace ama
