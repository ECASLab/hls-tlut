/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */
#pragma once
#include <complex>
#include <hls_math.h>
#include <iostream>
#include <valarray>

#include "Convolver.hpp"
#include "cores/arithmetic-exact.hpp"
#include "../utils/FFT_header.hpp"

namespace ama {
namespace hw {
namespace convolvers {

using namespace ama::hw;

/**
 * fft convolution class
 * It specialises the convolver into the exact convolution class.
 * @tparam T datatype to work with.
 * @tparam K kernel side size
 * @tparam O output window size
 * @tparam ADD add functor
 * @tparam ADD mult functor
 * @tparam M is the high of the input image
 * @tparam N is the width of the input image
 */
template <typename T, int K, int O = 2, class ADD = arithmetic::exact::Add<T>,
          class MULT = arithmetic::exact::Mult<T>, typename TI = half>
class Fft : public Convolver<T, K, O, ADD, MULT> {
 public:
  /**
   * Execute the exact implementation
   * @param input input to convolve with the kernel
   * @param kernel kernel to convolve with
   * @param output output window
   */
  virtual void Execute(
      const T input[Convolver<T, K, O, ADD, MULT>::windowsize]
                   [Convolver<T, K, O, ADD, MULT>::windowsize],
      const T kernel[Convolver<T, K, O, ADD, MULT>::kernelsize]
                    [Convolver<T, K, O, ADD, MULT>::kernelsize],
      T output[Convolver<T, K, O, ADD, MULT>::outputsize]
              [Convolver<T, K, O, ADD, MULT>::outputsize]);

 private:
  typedef TI ProcessType;
  /**
   * @brief Matrix fft convolution for array
   * It performs a lineal fft for array of complex numbers.
   * @param x input array
   * @param N size of the array
   */
  void fft_1D(Complex<ProcessType> x[Convolver<T, K, O, ADD, MULT>::windowsize]);
  /**
   * @brief Matrix fft convolution for array
   * It performs a lineal fft for array of complex numbers.
   * @param input input matrix
   * @param N number of columns
   * @param M number of rows
   */
  void fft_2D(
      Complex<ProcessType> input[Convolver<T, K, O, ADD, MULT>::windowsize]
                                [Convolver<T, K, O, ADD, MULT>::windowsize]);

  /**
   * @brief Matrix inverse fft convolution for array
   * It performs a lineal fft for array of complex numbers.
   * @param x input array
   */
  void ifft(Complex<ProcessType> x[Convolver<T, K, O, ADD, MULT>::windowsize]);

  /**
   * @brief Matrix fft convolution for array
   * It performs a lineal fft for array of complex numbers.
   * @param input input matrix
   * @param N number of columns
   * @param M number of rows
   */
  void ifft_2D(
      Complex<ProcessType> input[Convolver<T, K, O, ADD, MULT>::windowsize]
                                [Convolver<T, K, O, ADD, MULT>::windowsize]);

  /**
   * @brief Matrix padding
   * It performs the padding on a matrix for a kernel size k.
   * @param input_kernel Matrix original kernel
   * @param output_image kernel with the padding applied
   */
  void paddkernel_FFT(
      const T input_kernel[Convolver<T, K, O, ADD, MULT>::kernelsize]
                          [Convolver<T, K, O, ADD, MULT>::kernelsize],
      Complex<ProcessType>
          output_image[Convolver<T, K, O, ADD, MULT>::windowsize]
                      [Convolver<T, K, O, ADD, MULT>::windowsize]);

  /**
   * @brief Matrix padding
   * It performs the padding on a matrix for a kernel size k.
   * @param input_kernel Matrix original kernel
   * @param output_image kernel with the padding applied
   */
  void Real_to_complex(
      const T input_image[Convolver<T, K, O, ADD, MULT>::windowsize]
                         [Convolver<T, K, O, ADD, MULT>::windowsize],
      Complex<ProcessType>
          output_image[Convolver<T, K, O, ADD, MULT>::windowsize]
                      [Convolver<T, K, O, ADD, MULT>::windowsize]);
};

template <typename T, int K, int O, class ADD, class MULT, typename TI>
inline void Fft<T, K, O, ADD, MULT, TI>::Execute(
    const T input[Convolver<T, K, O, ADD, MULT>::windowsize]
                 [Convolver<T, K, O, ADD, MULT>::windowsize],
    const T kernel[Convolver<T, K, O, ADD, MULT>::kernelsize]
                  [Convolver<T, K, O, ADD, MULT>::kernelsize],
    T output[Convolver<T, K, O, ADD, MULT>::outputsize]
            [Convolver<T, K, O, ADD, MULT>::outputsize]) {
  constexpr int M = Convolver<T, K, O, ADD, MULT>::windowsize;
  constexpr int N = Convolver<T, K, O, ADD, MULT>::windowsize;
  constexpr int MO = Convolver<T, K, O, ADD, MULT>::outputsize;
  static_assert((M > 0) && ((M & (M - 1)) ==  0), "FFT only supports input window power of 2");
  Complex<ProcessType> a[M][N];
  Real_to_complex(input, a);

  fft_2D(a);

  Complex<ProcessType> b[M][N];

  paddkernel_FFT(kernel, b);

  fft_2D(b);

  const int i_mid = (M / 2) - 1;
  const int j_mid = (M / 2) - 1;
  // Hadamard product

  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
      a[i][j] = a[i][j] * b[i][j];
    }
  }
  ifft_2D(a);

  // fixes the quadrants of output

  T output_buffer[M][N];

  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
       if (i <= i_mid && j <= j_mid) {
         output_buffer[i + i_mid + 1][j + j_mid + 1] = a[i][j].real();
       } else if (i <= i_mid && j >= j_mid) {
         output_buffer[j][i] = a[i][j].real();
       } else if (i >= i_mid && j <= j_mid) {
         output_buffer[j][i] = a[i][j].real();
       } else if (i > i_mid && j > j_mid) {
         output_buffer[i - i_mid - 1][j - j_mid - 1] = a[i][j].real();
       }
    }
  }
  int crop_area = (M - MO) / 2;

  for (int i{0}; i < MO; ++i) {
    for (int j{0}; j < MO; ++j) {
      output[i][j] = output_buffer[i + crop_area][j + crop_area];
    }
  }
}

template <typename C, int K, int O, class ADD, class MULT, typename TI>
inline void Fft<C, K, O, ADD, MULT, TI>::fft_1D(
    Complex<Fft<C, K, O, ADD, MULT, TI>::ProcessType> x[Convolver<C, K, O, ADD, MULT>::windowsize]) {
  // DFT
  unsigned int N = Convolver<C, K, O, ADD, MULT>::windowsize, k = N, n;
  const ProcessType thetaT = kPI / N;
  Complex<ProcessType> phiT{hls::cos(thetaT), -hls::sin(thetaT)}, T;
  while (k > 1) {
#pragma HLS loop_tripcount min=1 max=4
    n = k;
    k >>= 1;
    phiT = phiT * phiT;
    T = 1;
    for (unsigned int l = 0; l < k; l++) {
#pragma HLS loop_tripcount min=1 max=8
      for (unsigned int a = l; a < N; a += n) {
#pragma HLS loop_tripcount min=1 max=8
        unsigned int b = a + k;
        auto t = x[a] - x[b];
        x[a] += x[b];
        x[b] = t * T;
      }
      T *= phiT;
    }
  }
  // Decimate
  const auto m = static_cast<unsigned int>(log2(N));
  for (unsigned int a = 0; a < N; a++) {
#pragma HLS loop_tripcount min=1 max=4
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

template <typename T, int K, int O, class ADD, class MULT, typename TI>
inline void Fft<T, K, O, ADD, MULT, TI>::fft_2D(
    Complex<Fft<T, K, O, ADD, MULT, TI>::ProcessType>
        input[Convolver<T, K, O, ADD, MULT>::windowsize]
             [Convolver<T, K, O, ADD, MULT>::windowsize]) {
  constexpr int M = Convolver<T, K, O, ADD, MULT>::windowsize;
  constexpr int N = Convolver<T, K, O, ADD, MULT>::windowsize;
  Complex<ProcessType> input_arr[M];
  // Row FFT
  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
      input_arr[j] = input[i][j];
    }
    fft_1D(input_arr);
    for (int j{0}; j < N; ++j) {
      input[i][j] = input_arr[j];
    }
  }
  // Columns FFT
  for (int i{0}; i < N; ++i) {
    for (int j{0}; j < M; ++j) {
      input_arr[j] = input[j][i];
    }
    fft_1D(input_arr);
    for (int j{0}; j < M; ++j) {
      input[j][i] = input_arr[j];
    }
  }
}

template <typename T, int K, int O, class ADD, class MULT, typename TI>
inline void Fft<T, K, O, ADD, MULT, TI>::ifft(
    Complex<Fft<T, K, O, ADD, MULT, TI>::ProcessType> x[Convolver<T, K, O, ADD, MULT>::windowsize]) {
  // conjugate the complex numbers
  constexpr int M = Convolver<T, K, O, ADD, MULT>::windowsize;
  for (int j{0}; j < M; ++j) {
    x[j] = std::conj(x[j]);
  }

  // forward fft
  fft_1D(x);

  // conjugate the complex numbers again
  for (int j{0}; j < M; ++j) {
    x[j] = std::conj(x[j]);
  }

  // scale the numbers
  ProcessType scale = 1. / M;
  for (int j{0}; j < M; ++j) {
    x[j] = {x[j].real()*scale,x[j].imag()*scale};
  }
}

template <typename T, int K, int O, class ADD, class MULT, typename TI>
inline void Fft<T, K, O, ADD, MULT, TI>::ifft_2D(
    Complex<Fft<T, K, O, ADD, MULT, TI>::ProcessType>
        input[Convolver<T, K, O, ADD, MULT>::windowsize]
             [Convolver<T, K, O, ADD, MULT>::windowsize]) {
  constexpr int M = Convolver<T, K, O, ADD, MULT>::windowsize;
  constexpr int N = Convolver<T, K, O, ADD, MULT>::windowsize;
  Complex<ProcessType> input_arr[M];

  // Columns iFFT
  for (int i{0}; i < N; ++i) {
    for (int j{0}; j < M; ++j) {
      input_arr[j] = input[j][i];
    }
    ifft(input_arr);
    for (int j{0}; j < M; ++j) {
      input[j][i] = input_arr[j];
    }
  }

  // Row iFFT
  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
      input_arr[j] = input[i][j];
    }
    ifft(input_arr);
    for (int j{0}; j < N; ++j) {
      input[i][j] = input_arr[j];
    }
  }
}

template <typename T, int K, int O, class ADD, class MULT, typename TI>
inline void Fft<T, K, O, ADD, MULT, TI>::paddkernel_FFT(
    const T input_kernel[Convolver<T, K, O, ADD, MULT>::kernelsize]
                        [Convolver<T, K, O, ADD, MULT>::kernelsize],
    Complex<Fft<T, K, O, ADD, MULT, TI>::ProcessType>
        output_image[Convolver<T, K, O, ADD, MULT>::windowsize]
                    [Convolver<T, K, O, ADD, MULT>::windowsize]) {
  constexpr int M = Convolver<T, K, O, ADD, MULT>::windowsize;
  constexpr int N = Convolver<T, K, O, ADD, MULT>::windowsize;
  int start[2] = {0};
  const int offset = (K + 1) / 2;

  start[0] = (M / 2) - offset;
  start[1] = (N / 2) - offset;

  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
      output_image[i][j] = 0;
    }
  }

  for (int i{0}; i < K; ++i) {
    for (int j{0}; j < K; ++j) {
      output_image[start[0] + i][start[1] + j] = input_kernel[i][j];
    }
  }
}

template <typename T, int K, int O, class ADD, class MULT, typename TI>
inline void Fft<T, K, O, ADD, MULT, TI>::Real_to_complex(
    const T input_image[Convolver<T, K, O, ADD, MULT>::windowsize]
                       [Convolver<T, K, O, ADD, MULT>::windowsize],
    Complex<Fft<T, K, O, ADD, MULT, TI>::ProcessType>
        output_image[Convolver<T, K, O, ADD, MULT>::windowsize]
                    [Convolver<T, K, O, ADD, MULT>::windowsize]) {
  constexpr int M = Convolver<T, K, O, ADD, MULT>::windowsize;
  constexpr int N = Convolver<T, K, O, ADD, MULT>::windowsize;
  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
      output_image[i][j] = input_image[i][j];
    }
  }
}
} /* namespace convolvers */
} /* namespace hw */
} /* namespace ama */
