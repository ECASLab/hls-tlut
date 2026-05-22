/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>

#include "Convolver.hpp"
#include "cores/arithmetic-exact.hpp"

namespace ama {
namespace hw {
namespace convolvers {

using namespace ama::hw;
static constexpr int kWinKernel7 = 7;

/**
 * Winograd convolution class
 * It specialises the convolver into the exact convolution class.
 * @tparam T datatype to work with.
 * @tparam K kernel side size: fixed to 7
 * @tparam O output window size
 * @tparam ADD add functor
 * @tparam ADD mult functor
 */
template <typename T, int O, class ADD, class MULT>
class Winograd<T, kWinKernel7, O, ADD, MULT>
    : public Convolver<T, kWinKernel7, O, ADD, MULT> {
 public:
  /**
   * Execute the exact implementation
   * @param window input window to convolve with the kernel
   * @param kernel kernel to convolve with
   * @param output output window
   */
  virtual void Execute(
      const T window[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                    [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
      const T kernel[Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize]
                    [Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize],
      T output[Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]
              [Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]) override;

 private:
  typedef ap_fixed<T::width * 2, T::width> ProcessType;

  /**
   * Performs the transformation of the Input into the Winograd7 Domain
   * The operation performed is BtZB
   * @param w input in space domain
   * @param w_w transformed input into Winograd7 domain
   */
  void TransformInput(
      const T w[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
               [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
      ProcessType w_w[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                     [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]);

  /**
   * Performs the transformation of the kernel into the Winograd7 Domain
   * The operation performed is GfGt
   * @param k kernel in space domain
   * @param w_k transformed kernel into Winograd7 domain
   */
  void TransformKernel(
      const T k[Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize]
               [Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize],
      ProcessType w_k[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                     [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]);

  /**
   * Performs the Hadamard product between two spectrums
   * @param w_k kernel in Winograd7 domain
   * @param w_w input in Winograd7 domain
   * @param w_kw output in Winograd7 domain
   */
  void Hadamard(
      const ProcessType
          w_k[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
             [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
      const ProcessType
          w_w[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
             [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
      ProcessType w_kw[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                      [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]);

  template <int M1, int N, int N2, typename T1, typename T2, typename T3>
  void MatMultiply(const T1 a[M1][N], const T2 b[N][N2], T3 c[M1][N2]);

  /**
   * Performs the transformation of the output into the space Domain
   * The operation performed is AtHA
   * @param w_h output in Winograd7 domain
   * @param h transformed output into space domain
   */
  void DetransformOutput(
      const ProcessType
          w_h[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
             [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
      T h[Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]
         [Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]);
};

template <typename T, int O, class ADD, class MULT>
inline void Winograd<T, kWinKernel7, O, ADD, MULT>::Execute(
    const T window[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                  [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
    const T kernel[Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize]
                  [Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize],
    T output[Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]
            [Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]) {
  static_assert(O == 2, "Winograd only supports K = 7 and O = 2");

  ProcessType w_window[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                      [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize];
  ProcessType w_kernel[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                      [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize];
  ProcessType w_output[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
                      [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize];

  /* Transform */
  TransformInput(window, w_window);
  TransformKernel(kernel, w_kernel);

  /* Convolve */
  Hadamard(w_kernel, w_window, w_output);

  /* Transform back */
  DetransformOutput(w_output, output);
}

template <typename T, int O, class ADD, class MULT>
template <int M1, int N, int N2, typename T1, typename T2, typename T3>
inline void Winograd<T, kWinKernel7, O, ADD, MULT>::MatMultiply(
    const T1 a[M1][N], const T2 b[N][N2], T3 c[M1][N2]) {
winograd_exact_matmul_i:
  for (int i{0}; i < M1; ++i) {
  winograd_exact_matmul_j:
    for (int j{0}; j < N2; ++j) {
      c[i][j] = 0;
    winograd_exact_matmul_k:
      for (int k{0}; k < N; ++k) {
        c[i][j] += a[i][k] * b[k][j];
      }
    }
  }
}

template <typename T, int O, class ADD, class MULT>
inline void Winograd<T, kWinKernel7, O, ADD, MULT>::DetransformOutput(
    const Winograd<T, kWinKernel7, O, ADD, MULT>::ProcessType
        w_h[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
           [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
    T h[Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]
       [Convolver<T, kWinKernel7, O, ADD, MULT>::outputsize]) {
  // A.T * H * A
  const int kAtRows = 2;
  const int kAtCols = 8;

  ProcessType At[kAtRows][kAtCols] = {{1, 1, 1, 1, 1, 1, 1, 0},
                                      {0, 1, -1, 2, -2, 0.5, -0.5, 1}};
  ProcessType A[kAtCols][kAtRows] = {{1, 0},  {1, 1},   {1, -1},   {1, 2},
                                     {1, -2}, {1, 0.5}, {1, -0.5}, {0, 1}};
  ProcessType hi[kAtRows][Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize];

  MatMultiply<kAtRows, kAtCols,
              Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize>(At, w_h, hi);
  MatMultiply<kAtRows, Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize,
              kAtRows>(hi, A, h);
}

template <typename T, int O, class ADD, class MULT>
inline void Winograd<T, kWinKernel7, O, ADD, MULT>::Hadamard(
    const Winograd<T, kWinKernel7, O, ADD, MULT>::ProcessType
        w_k[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
           [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
    const Winograd<T, kWinKernel7, O, ADD, MULT>::ProcessType
        w_w[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
           [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
    Winograd<T, kWinKernel7, O, ADD, MULT>::ProcessType
        w_kw[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
            [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]) {
winograd_exact_hadamard_i:
  for (int i{0}; i < Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize; ++i) {
  winograd_exact_hadamard_j:
    for (int j{0}; j < Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize;
         ++j) {
      w_kw[i][j] = w_k[i][j] * w_w[i][j];
    }
  }
}

template <typename T, int O, class ADD, class MULT>
inline void Winograd<T, kWinKernel7, O, ADD, MULT>::TransformInput(
    const T w[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
             [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize],
    Winograd<T, kWinKernel7, O, ADD, MULT>::ProcessType
        w_w[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
           [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]) {
  // B.T * W * B
  const int kBFields = 8;

  ProcessType Bt[kBFields][kBFields] = {{1, 0, -5.25, 0, 5.25, 0, -1, 0},
                                        {0, 1, 1, -4.25, -4.25, 1, 1, 0},
                                        {0, -1, 1, 4.25, -4.25, -1, 1, 0},
                                        {0, 0.5, 0.25, -2.5, -1.25, 2, 1, 0},
                                        {0, -0.5, 0.25, 2.5, -1.25, -2, 1, 0},
                                        {0, 2, 4, -2.5, -5, 0.5, 1, 0},
                                        {0, -2, 4, 2.5, -5, -0.5, 1, 0},
                                        {0, -1, 0, 5.25, 0, -5.25, 0, 1}};
  ProcessType B[kBFields][kBFields] = {
      {1, 0, 0, 0, 0, 0, 0, 0},
      {0, 1, -1, 0.5, -0.5, 2, -2, -1},
      {-5.25, 1, 1, 0.25, 0.25, 4, 4, 0},
      {0, -4.25, 4.25, -2.5, 2.5, -2.5, 2.5, 5.25},
      {5.25, -4.25, -4.25, -1.25, -1.25, -5, -5, 0},
      {0, 1, -1, 2, -2, 0.5, -0.5, -5.25},
      {-1, 1, 1, 1, 1, 1, 1, 0},
      {0, 0, 0, 0, 0, 0, 0, 1}};
  ProcessType wi[kBFields][kBFields];

  MatMultiply<kBFields, kBFields,
              Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize>(Bt, w, wi);
  MatMultiply<kBFields, Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize,
              kBFields>(wi, B, w_w);
}

template <typename T, int O, class ADD, class MULT>
inline void Winograd<T, kWinKernel7, O, ADD, MULT>::TransformKernel(
    const T k[Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize]
             [Convolver<T, kWinKernel7, O, ADD, MULT>::kernelsize],
    Winograd<T, kWinKernel7, O, ADD, MULT>::ProcessType
        w_k[Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]
           [Convolver<T, kWinKernel7, O, ADD, MULT>::windowsize]) {
  // G * K * G.T
  const int kGRows = 8;
  const int kGCols = 7;

  ProcessType G[kGRows][kGCols] = {
      {1, 0, 0, 0, 0, 0, 0},
      {-0.22, -0.22, -0.22, -0.22, -0.22, -0.22, -0.22},
      {-0.22, 0.22, -0.22, 0.22, -0.22, 0.22, -0.22},
      {0.011, 0.022, 0.044, 0.088, 0.178, 0.356, 0.711},
      {0.011, -0.022, 0.044, -0.088, 0.178, -0.356, 0.711},
      {0.711, 0.356, 0.178, 0.088, 0.044, 0.022, 0.011},
      {0.711, -0.356, 0.178, -0.088, 0.044, -0.022, 0.011},
      {0, 0, 0, 0, 0, 0, 1}};
  ProcessType Gt[kGCols][kGRows] = {
      {1, -0.22, -0.22, 0.011, 0.011, 0.711, 0.711, 0},
      {0, -0.22, 0.22, 0.022, -0.022, 0.356, -0.356, 0},
      {0, -0.22, -0.22, 0.044, 0.044, 0.178, 0.178, 0},
      {0, -0.22, 0.22, 0.088, -0.088, 0.088, -0.088, 0},
      {0, -0.22, -0.22, 0.178, 0.178, 0.044, 0.044, 0},
      {0, -0.22, 0.22, 0.356, -0.356, 0.022, -0.022, 0},
      {0, -0.22, -0.22, 0.711, 0.711, 0.011, 0.011, 1}};
  ProcessType ki[kGRows][7];

  MatMultiply<kGRows, kGCols, 7>(G, k, ki);
  MatMultiply<kGRows, 7, kGRows>(ki, Gt, w_k);
}

} /* namespace convolvers */
} /* namespace hw */
} /* namespace ama */
