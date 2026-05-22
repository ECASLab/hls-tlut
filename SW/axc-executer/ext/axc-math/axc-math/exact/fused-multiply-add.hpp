/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

namespace axc {
namespace arithmetic {
namespace exact {

/**
 * FMA functor
 * This implements the exact version of the FMA
 * @tparam T datatype
 */
template <typename T>
class FMA {
 public:
  /**
   * Operator() overload
   * @param lhs left hand operand
   * @param mhs middule hand operand
   * @param rhs right hand operand
   */
  T operator()(const T lhs, const T mhs, const T rhs) {
#pragma HLS INLINE
    return lhs * mhs + rhs;
  }
};

}  // namespace exact
}  // namespace arithmetic
}  // namespace axc
