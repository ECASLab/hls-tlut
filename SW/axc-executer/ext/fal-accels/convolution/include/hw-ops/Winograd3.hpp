/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/*
 * Git:
 * https://github.com/lirui-shanghaitech/A-convolution-kernel-implemented-by-Vivado-HLS
 * Paper: https://ceca.pku.edu.cn/media/lw/6940b5b0e09259131ff19334f3efeecd.pdf
 *
 * https://github.com/xliu0709/WinoCNN
 */

#pragma once

#include <ap_fixed.h>

#include "Convolver.hpp"
#include "cores/arithmetic-exact.hpp"

namespace ama {
namespace hw {
namespace convolvers {

using namespace ama::hw;

/**
 * Winograd convolution class
 * It specialises the convolver into the exact convolution class.
 * @tparam T datatype to work with.
 * @tparam K kernel side size
 * @tparam O output window size
 * @tparam ADD add functor
 * @tparam ADD mult functor
 */
template <typename T, int K, int O = 2, class ADD = arithmetic::exact::Add<T>,
          class MULT = arithmetic::exact::Mult<T>>
class Winograd : public Convolver<T, K, O, ADD, MULT> {
 public:
  /**
   * Execute the exact implementation
   * @param window input window to convolve with the kernel
   * @param kernel kernel to convolve with
   * @param output output window
   */
  virtual void Execute(
      const T window[Convolver<T, K, O, ADD, MULT>::windowsize]
                    [Convolver<T, K, O, ADD, MULT>::windowsize],
      const T kernel[Convolver<T, K, O, ADD, MULT>::kernelsize]
                    [Convolver<T, K, O, ADD, MULT>::kernelsize],
      T output[Convolver<T, K, O, ADD, MULT>::outputsize]
              [Convolver<T, K, O, ADD, MULT>::outputsize]) override;

 private:
  typedef ap_fixed<T::width * 2, T::width> ProcessType;

  /**
   * Performs the transformation of the Input into the Winograd Domain
   * The operation performed is BtZB
   * @param w input in space domain
   * @param w_w transformed input into Winograd domain
   */
  void TransformInput(
      const T w[Convolver<T, K, O, ADD, MULT>::windowsize]
               [Convolver<T, K, O, ADD, MULT>::windowsize],
      ProcessType w_w[Convolver<T, K, O, ADD, MULT>::windowsize]
                     [Convolver<T, K, O, ADD, MULT>::windowsize]);

  /**
   * Performs the transformation of the kernel into the Winograd Domain
   * The operation performed is GfGt
   * @param k kernel in space domain
   * @param w_k transformed kernel into Winograd domain
   */
  void TransformKernel(
      const T k[Convolver<T, K, O, ADD, MULT>::kernelsize]
               [Convolver<T, K, O, ADD, MULT>::kernelsize],
      ProcessType w_k[Convolver<T, K, O, ADD, MULT>::windowsize]
                     [Convolver<T, K, O, ADD, MULT>::windowsize]);

  /**
   * Performs the Hadamard product between two spectrums
   * @param w_k kernel in Winograd domain
   * @param w_w input in Winograd domain
   * @param w_kw output in Winograd domain
   */
  void Hadamard(
      const ProcessType w_k[Convolver<T, K, O, ADD, MULT>::windowsize]
                           [Convolver<T, K, O, ADD, MULT>::windowsize],
      const ProcessType w_w[Convolver<T, K, O, ADD, MULT>::windowsize]
                           [Convolver<T, K, O, ADD, MULT>::windowsize],
      ProcessType w_kw[Convolver<T, K, O, ADD, MULT>::windowsize]
                      [Convolver<T, K, O, ADD, MULT>::windowsize]);

  /**
   * Performs the transformation of the output into the space Domain
   * The operation performed is AtHA
   * @param w_h output in Winograd domain
   * @param h transformed output into space domain
   */
  void DetransformOutput(
      const ProcessType w_h[Convolver<T, K, O, ADD, MULT>::windowsize]
                           [Convolver<T, K, O, ADD, MULT>::windowsize],
      T h[Convolver<T, K, O, ADD, MULT>::outputsize]
         [Convolver<T, K, O, ADD, MULT>::outputsize]);
};

template <typename T, int K, int O, class ADD, class MULT>
inline void Winograd<T, K, O, ADD, MULT>::Execute(
    const T window[Convolver<T, K, O, ADD, MULT>::windowsize]
                  [Convolver<T, K, O, ADD, MULT>::windowsize],
    const T kernel[Convolver<T, K, O, ADD, MULT>::kernelsize]
                  [Convolver<T, K, O, ADD, MULT>::kernelsize],
    T output[Convolver<T, K, O, ADD, MULT>::outputsize]
            [Convolver<T, K, O, ADD, MULT>::outputsize]) {
  static_assert(K == 3 && O == 2, "Winograd only supports K = 3 and O = 2");

  ProcessType w_window[Convolver<T, K, O, ADD, MULT>::windowsize]
                      [Convolver<T, K, O, ADD, MULT>::windowsize];
  ProcessType w_kernel[Convolver<T, K, O, ADD, MULT>::windowsize]
                      [Convolver<T, K, O, ADD, MULT>::windowsize];
  ProcessType w_output[Convolver<T, K, O, ADD, MULT>::windowsize]
                      [Convolver<T, K, O, ADD, MULT>::windowsize];

  /* Transform */
  TransformInput(window, w_window);
  TransformKernel(kernel, w_kernel);

  /* Convolve */
  Hadamard(w_kernel, w_window, w_output);

  /* Transform back */
  DetransformOutput(w_output, output);
}

template <typename T, int K, int O, class ADD, class MULT>
inline void Winograd<T, K, O, ADD, MULT>::DetransformOutput(
    const Winograd<T, K, O, ADD, MULT>::ProcessType
        w_h[Convolver<T, K, O, ADD, MULT>::windowsize]
           [Convolver<T, K, O, ADD, MULT>::windowsize],
    T h[Convolver<T, K, O, ADD, MULT>::outputsize]
       [Convolver<T, K, O, ADD, MULT>::outputsize]) {
  h[0][0] = w_h[0][0] + w_h[0][1] + w_h[0][2] + w_h[1][0] + w_h[1][1] +
            w_h[1][2] + w_h[2][0] + w_h[2][1] + w_h[2][2];
  h[0][1] = w_h[0][1] - w_h[0][2] + w_h[0][3] + w_h[1][1] - w_h[1][2] +
            w_h[1][3] + w_h[2][1] - w_h[2][2] + w_h[2][3];
  h[1][0] = w_h[1][0] + w_h[1][1] + w_h[1][2] - w_h[2][0] - w_h[2][1] -
            w_h[2][2] + w_h[3][0] + w_h[3][1] + w_h[3][2];
  h[1][1] = w_h[1][1] - w_h[1][2] + w_h[1][3] - w_h[2][1] + w_h[2][2] -
            w_h[2][3] + w_h[3][1] - w_h[3][2] + w_h[3][3];
}

template <typename T, int K, int O, class ADD, class MULT>
inline void Winograd<T, K, O, ADD, MULT>::Hadamard(
    const Winograd<T, K, O, ADD, MULT>::ProcessType
        w_k[Convolver<T, K, O, ADD, MULT>::windowsize]
           [Convolver<T, K, O, ADD, MULT>::windowsize],
    const Winograd<T, K, O, ADD, MULT>::ProcessType
        w_w[Convolver<T, K, O, ADD, MULT>::windowsize]
           [Convolver<T, K, O, ADD, MULT>::windowsize],
    Winograd<T, K, O, ADD, MULT>::ProcessType
        w_kw[Convolver<T, K, O, ADD, MULT>::windowsize]
            [Convolver<T, K, O, ADD, MULT>::windowsize]) {
winograd_exact_hadamard_i:
  for (int i{0}; i < Convolver<T, K, O, ADD, MULT>::windowsize; ++i) {
  winograd_exact_hadamard_j:
    for (int j{0}; j < Convolver<T, K, O, ADD, MULT>::windowsize; ++j) {
      w_kw[i][j] = w_k[i][j] * w_w[i][j];
    }
  }
}

template <typename T, int K, int O, class ADD, class MULT>
inline void Winograd<T, K, O, ADD, MULT>::TransformInput(
    const T w[Convolver<T, K, O, ADD, MULT>::windowsize]
             [Convolver<T, K, O, ADD, MULT>::windowsize],
    Winograd<T, K, O, ADD, MULT>::ProcessType
        w_w[Convolver<T, K, O, ADD, MULT>::windowsize]
           [Convolver<T, K, O, ADD, MULT>::windowsize]) {
  /*
   * TODO: Factorise operations. It may lead to overflow if not taken into
   * account
   */
  w_w[0][0] = w[0][0] - w[0][2] - w[2][0] + w[2][2];
  w_w[0][1] = w[0][1] + w[0][2] - w[2][1] - w[2][2];
  w_w[0][2] = -w[0][1] + w[0][2] + w[2][1] - w[2][2];
  w_w[0][3] = -w[0][1] + w[0][3] + w[2][1] - w[2][3];
  w_w[1][0] = w[1][0] - w[1][2] + w[2][0] - w[2][2];
  w_w[1][1] = w[1][1] + w[1][2] + w[2][1] + w[2][2];
  w_w[1][2] = -w[1][1] + w[1][2] - w[2][1] + w[2][2];
  w_w[1][3] = -w[1][1] + w[1][3] - w[2][1] + w[2][3];
  w_w[2][0] = -w[1][0] + w[1][2] + w[2][0] - w[2][2];
  w_w[2][1] = -w[1][1] - w[1][2] + w[2][1] + w[2][2];
  w_w[2][2] = w[1][1] - w[1][2] - w[2][1] + w[2][2];
  w_w[2][3] = w[1][1] - w[1][3] - w[2][1] + w[2][3];
  w_w[3][0] = -w[1][0] + w[1][2] + w[3][0] - w[3][2];
  w_w[3][1] = -w[1][1] - w[1][2] + w[3][1] + w[3][2];
  w_w[3][2] = w[1][1] - w[1][2] - w[3][1] + w[3][2];
  w_w[3][3] = w[1][1] - w[1][3] - w[3][1] + w[3][3];
}

template <typename T, int K, int O, class ADD, class MULT>
inline void Winograd<T, K, O, ADD, MULT>::TransformKernel(
    const T k[Convolver<T, K, O, ADD, MULT>::kernelsize]
             [Convolver<T, K, O, ADD, MULT>::kernelsize],
    Winograd<T, K, O, ADD, MULT>::ProcessType
        w_k[Convolver<T, K, O, ADD, MULT>::windowsize]
           [Convolver<T, K, O, ADD, MULT>::windowsize]) {
  /* TODO: Factorise operations */
  w_k[0][0] = k[0][0];
  w_k[0][1] = (k[0][0] + k[0][1] + k[0][2]) >> 1;
  w_k[0][2] = (k[0][0] - k[0][1] + k[0][2]) >> 1;
  w_k[0][3] = k[0][2];
  w_k[1][0] = (k[0][0] + k[1][0] + k[2][0]) >> 1;
  w_k[1][1] = (k[0][0] + k[0][1] + k[0][2] + k[1][0] + k[1][1] + k[1][2] +
               k[2][0] + k[2][1] + k[2][2]) >>
              2;
  w_k[1][2] = (k[0][0] - k[0][1] + k[0][2] + k[1][0] - k[1][1] + k[1][2] +
               k[2][0] - k[2][1] + k[2][2]) >>
              2;
  w_k[1][3] = (k[0][2] + k[1][2] + k[2][2]) >> 1;
  w_k[2][0] = (k[0][0] - k[1][0] + k[2][0]) >> 1;
  w_k[2][1] = (k[0][0] + k[0][1] + k[0][2] - k[1][0] - k[1][1] - k[1][2] +
               k[2][0] + k[2][1] + k[2][2]) >>
              2;
  w_k[2][2] = (k[0][0] - k[0][1] + k[0][2] - k[1][0] + k[1][1] - k[1][2] +
               k[2][0] - k[2][1] + k[2][2]) >>
              2;
  w_k[2][3] = (k[0][2] - k[1][2] + k[2][2]) >> 1;
  w_k[3][0] = k[2][0];
  w_k[3][1] = (k[2][0] + k[2][1] + k[2][2]) >> 1;
  w_k[3][2] = (k[2][0] - k[2][1] + k[2][2]) >> 1;
  w_k[3][3] = k[2][2];
}

} /* namespace convolvers */
} /* namespace hw */
} /* namespace ama */
