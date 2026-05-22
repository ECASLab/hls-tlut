/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

namespace axc {
namespace arithmetic {
namespace exact {

/**
 * Addition functor
 * This implements the exact version of the addition
 * @tparam T datatype
 */
template <typename T>
class Add {
 public:
  /**
   * Operator() overload
   * @param lhs left hand operand
   * @param rhs right hand operand
   */
  T operator()(const T lhs, const T rhs) {
#pragma HLS INLINE
    return lhs + rhs;
  }
};

}  // namespace exact
}  // namespace arithmetic
}  // namespace axc
