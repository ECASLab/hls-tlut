/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once
#include <complex>
#include <iostream>
#include <valarray>
#include "IFFT.hpp"
#include "../utils/FFT_header.hpp"

namespace ama {
namespace sw {

/**
 * @brief Matrix fft convolution for array
 * It performs a lineal fft for array of complex numbers.
 * @param input input matrix
 * @param N number of columns
 * @param M number of rows
 */

template <typename T, int M, int N>

void ifft_2D(Complex<T> input[M][N]) {
  Complex<T> input_arr[M] = {0};

  // Columns iFFT
  for (int i{0}; i < N; ++i) {
    for (int j{0}; j < M; ++j) {
      input_arr[j] = input[j][i];
    }
    CArray<T> data(input_arr, M);
    ama::sw::ifft<T>(data);
    for (int j{0}; j < M; ++j) {
      input[j][i] = data[j];
    }
  }

  // Row iFFT
  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
      input_arr[j] = input[i][j];
    }
    CArray<T> data(input_arr, N);
    ama::sw::ifft<T>(data);
    for (int j{0}; j < N; ++j) {
      input[i][j] = data[j];
    }
  }
}
}  // namespace sw
}  // namespace ama
