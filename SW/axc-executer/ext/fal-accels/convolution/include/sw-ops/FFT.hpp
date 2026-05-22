/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once
#include <complex>
#include <iostream>
#include <valarray>
#include "../utils/FFT_header.hpp"

namespace ama {
namespace sw {

/**
 * @brief Matrix fft convolution for array
 * It performs a lineal fft for array of complex numbers.
 * @param x input array
 * @param N size of the array
 */

template <typename C = float>

void fft(CArray<C> &x) {
  // DFT
  unsigned int N = x.size(), k = N, n;
   C thetaT = kPI / N;
   Complex<C> phiT{cos(thetaT), -sin(thetaT)}, T;

  while (k > 1) {
    n = k;
    k >>= 1;
    phiT = phiT * phiT;
    T = 1.0L;
    for (unsigned int l = 0; l < k; l++) {
      for (unsigned int a = l; a < N; a += n) {
        unsigned int b = a + k;
        auto t = x[a] - x[b];
        x[a] += x[b];
        x[b] = t * T;
      }
      T *= phiT;
    }
  }
  // Decimate
  auto m = static_cast<unsigned int>(log2(N));
  for (unsigned int a = 0; a < N; a++) {
    unsigned int b = a;
    // Reverse bits
    b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
    b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
    b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
    b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
    b = ((b >> 16) | (b << 16)) >> (32 - m);
    if (b > a) {
      auto t = x[a];
      x[a] = x[b];
      x[b] = t;
    }
  }
}
}  // namespace sw
}  // namespace ama
