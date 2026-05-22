/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

namespace axc {
namespace nonlinear {
namespace exact {

/**
 * ReLU functor
 * This implements the exact version of the Rectified Linear Unit
 * @tparam T datatype
 */
template <typename T>
class ReLU {
 public:
  T operator()(const T lhs) {
#pragma HLS INLINE
    return lhs > 0 ? lhs : T{0};
  }
};

}  // namespace exact
}  // namespace nonlinear
}  // namespace axc
