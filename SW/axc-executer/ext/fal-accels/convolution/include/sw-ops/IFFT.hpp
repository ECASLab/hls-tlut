/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once
#include <complex>
#include <iostream>
#include <valarray>
#include "FFT.hpp"
#include "../utils/FFT_header.hpp"

namespace ama {
namespace sw {

/**
 * @brief Matrix inverse fft convolution for array
 * It performs a lineal fft for array of complex numbers.
 * @param x input array
 */

template <typename T>

void ifft(CArray<T> &x) {
  // conjugate the complex numbers
  x = x.apply(std::conj);

  // forward fft
  ama::sw::fft<T>(x);

  // conjugate the complex numbers again
  x = x.apply(std::conj);

  // scale the numbers
  T scale = 1. / x.size();
  x *= scale;
}
}  // namespace sw
}  // namespace ama
