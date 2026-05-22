/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include "Convolver.hpp"
#include "cores/arithmetic-exact.hpp"

namespace ama {
namespace hw {
namespace convolvers {

using namespace ama::hw;

/**
 * Spatial convolution class
 * It specialises the convolver into the exact convolution class
 * @tparam T datatype to work with.
 * @tparam K kernel side size
 * @tparam O output window size
 * @tparam ADD add functor
 * @tparam ADD mult functor
 */
template <typename T, int K, int O = 2, class ADD = arithmetic::exact::Add<T>,
          class MULT = arithmetic::exact::Mult<T>>
class Spatial : public Convolver<T, K, O, ADD, MULT> {
 public:
  /**
   * Execute the implementation
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
  ADD add_{};
  MULT mult_{};
};

template <typename T, int K, int O, class ADD, class MULT>
inline void Spatial<T, K, O, ADD, MULT>::Execute(
    const T window[Convolver<T, K, O, ADD, MULT>::windowsize]
                  [Convolver<T, K, O, ADD, MULT>::windowsize],
    const T kernel[Convolver<T, K, O, ADD, MULT>::kernelsize]
                  [Convolver<T, K, O, ADD, MULT>::kernelsize],
    T output[Convolver<T, K, O, ADD, MULT>::outputsize]
            [Convolver<T, K, O, ADD, MULT>::outputsize]) {
  T sum = 0;
space_exact_execute_i:
  for (int i{0}; i < Convolver<T, K, O, ADD, MULT>::outputsize; ++i) {
  space_exact_execute_j:
    for (int j{0}; j < Convolver<T, K, O, ADD, MULT>::outputsize; ++j) {
    space_exact_execute_e:
      for (int e{0}; e < Convolver<T, K, O, ADD, MULT>::kernelsize; ++e) {
      space_exact_execute_r:
        for (int r{0}; r < Convolver<T, K, O, ADD, MULT>::kernelsize; ++r) {
          sum = add_(sum, mult_(kernel[e][r], window[i + e][j + r]));
        }
      }
      output[i][j] = sum;
      sum = 0;
    }
  }
}

} /* namespace convolvers */
} /* namespace hw */
} /* namespace ama */
