/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include "cores/arithmetic-exact.hpp"

namespace ama {
namespace hw {

/**
 * Convolver template class
 * It defines the interface for any convolution engine. It also defines
 * the window sizes as a function of the kernel size.
 * @tparam T datatype to work with.
 * @tparam K kernel side size
 * @tparam O output window size
 * @tparam ADD add functor
 * @tparam ADD mult functor
 */
template <typename T, int K, int O = 2, class ADD = arithmetic::exact::Add<T>,
          class MULT = arithmetic::exact::Mult<T>>
class Convolver {
 public:
  /*
   * Define some useful variables for implementation purposes
   * This may change according to the kernel.
   * FIXME: extend to kernels of different sizes
   */
  static const int kernelsize = K;
  static const int outputsize = O;
  static const int windowsize = K + O - 1;
  typedef T datatype;

  /**
   * Execute the exact implementation
   * @param window input window to convolve with the kernel
   * @param kernel kernel to convolve with
   * @param output output window
   */
  virtual void Execute(const T window[windowsize][windowsize],
                       const T kernel[kernelsize][kernelsize],
                       T output[outputsize][outputsize]) {}
};

/**
 * Begin of the metaprogramming loop
 * This struct will recurse until having a termination as a
 * specialisation.
 * @tparam N current iteration. In the first, it is equal to NT
 * @tparam NT total number of iterations
 * @tparam ENGINE class/module to iterate
 */
template <int N, int NT, class ENGINE>
struct ParallelConvolver {
  /**
   * Execute method
   * It exposes the execution static to interact with the ENGINE
   * FIXME: generalise for K != 3
   * @param input input window
   * @param kernel kernel to convolve with
   * @param output output window
   */
  static void Execute(
      typename ENGINE::datatype
          input[NT * ENGINE::outputsize + ENGINE::kernelsize - 1]
               [ENGINE::windowsize],
      typename ENGINE::datatype kernel[ENGINE::kernelsize][ENGINE::kernelsize],
      typename ENGINE::datatype output[NT * ENGINE::outputsize]
                                      [ENGINE::outputsize]) {
#pragma HLS INLINE /* Important! Inlining the execution allows parallelism */
    /* Execute PE */
    ENGINE op{};
    op.Execute(&input[ENGINE::outputsize * (N - 1)], kernel,
               &output[ENGINE::outputsize * (N - 1)]);

    /* Continue Loop - The next i = i - 1 */
    ParallelConvolver<(N - 1), NT, ENGINE>::Execute(input, kernel, output);
  }
};

/**
 * End of the metaprogramming loop
 */
template <int NT, class ENGINE>
struct ParallelConvolver<0, NT, ENGINE> {
  /**
   * Execute method
   * It exposes the execution static to interact with the ENGINE
   * @param input input window
   * @param kernel kernel to convolve with
   * @param output output window
   */
  static void Execute(
      typename ENGINE::datatype
          input[NT * ENGINE::outputsize + ENGINE::kernelsize - 1]
               [ENGINE::windowsize],
      typename ENGINE::datatype kernel[ENGINE::kernelsize][ENGINE::kernelsize],
      typename ENGINE::datatype output[NT * ENGINE::outputsize]
                                      [ENGINE::outputsize]) {
#pragma HLS INLINE /* Important! Inlining the execution allows parallelism */
    /* Do Nothing (terminate loop) */
  }
};

} /* namespace hw */
} /* namespace ama */
