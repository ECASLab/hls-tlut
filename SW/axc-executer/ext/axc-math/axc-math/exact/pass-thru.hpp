/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

namespace axc {
namespace nonlinear {
namespace exact {

/**
 * PassThru functor
 * This implements the exact version of the pass-thru
 * @tparam T datatype
 */
template <typename T>
class PassThru {
 public:
  T operator()(const T lhs) {
#pragma HLS INLINE
    return lhs;
  }
};

}  // namespace exact
}  // namespace nonlinear
}  // namespace axc
