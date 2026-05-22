/*
 * Copyright 2021-2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

namespace axc {
namespace arithmetic {
namespace exact {

/**
 * Division functor
 * This implements the exact version of the division
 * @tparam T datatype
 */
template <typename T>
class Divide {
 public:
  /**
   * Operator() overload
   * @param lhs left hand operand
   * @param rhs right hand operand
   */
  T operator()(const T lhs, const T rhs) {
#pragma HLS INLINE
    T recp = T{1.} / rhs;
    return lhs * recp;
  }
};

}  // namespace exact
}  // namespace arithmetic
}  // namespace axc
