/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once
#include "MatrixOperator.hpp"

namespace ama {
namespace hw {
namespace operators {

using namespace ama::hw;

/**
 * Matrix operator for matrix multiplication with Winograd algorithm
 * It usually works as matrix multiply operation but with a different logic
 * @tparam T datatype to work with.
 * @tparam M rows of a matrix
 * @tparam N columns of a matrix
 * @tparam ADD add functor. Defaults to the exact version
 * @tparam MULT mult functor. Defaults to the exact version
 * @tparam NL non-linear functor. Defaults to the pass-thru
 */
template <typename T, int M, int N, class ADD = arithmetic::exact::Add<T>,
          class MULT = arithmetic::exact::Mult<T>,
          class NL = arithmetic::exact::PassThru<T>>
class MatrixMultiplyAddWinograd
    : public MatrixOperator<T, M, N, ADD, MULT, NL> {
 public:
  /**
   * Execute the exact implementation for three-operand operators
   * The matrix multiplication in ap_fixed<> is better than the ap_int<>
   * without paying significant overhead.
   * It is constraint to MxN * NxM matrices
   * @param op_a input operand A
   * @param op_b input operand B
   * @param op_c output operand C
   */
  virtual void Execute(
      const T op_a[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
                  [MatrixOperator<T, M, N, ADD, MULT, NL>::columns],
      const T op_b[MatrixOperator<T, M, N, ADD, MULT, NL>::columns]
                  [MatrixOperator<T, M, N, ADD, MULT, NL>::rows],
      const T op_c[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
                  [MatrixOperator<T, M, N, ADD, MULT, NL>::rows],
      T op_d[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
            [MatrixOperator<T, M, N, ADD, MULT, NL>::rows]);

 private:
  ADD add_{};
  MULT mult_{};
  NL non_linearity_{};
};

template <typename T, int M, int N, class ADD, class MULT, class NL>
inline void MatrixMultiplyAddWinograd<T, M, N, ADD, MULT, NL>::Execute(
    const T op_a[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
                [MatrixOperator<T, M, N, ADD, MULT, NL>::columns],
    const T op_b[MatrixOperator<T, M, N, ADD, MULT, NL>::columns]
                [MatrixOperator<T, M, N, ADD, MULT, NL>::rows],
    const T op_c[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
                [MatrixOperator<T, M, N, ADD, MULT, NL>::rows],
    T op_d[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
          [MatrixOperator<T, M, N, ADD, MULT, NL>::rows]) {
  static_assert(M == 2 && N == 2, "Winograd supports only 2x2 matrices");

  T a1, b1, c1, d1;
  T a2, b2, c2, d2;

  a1 = op_a[0][0];
  b1 = op_a[0][1];
  c1 = op_a[1][0];
  d1 = op_a[1][1];

  a2 = op_b[0][0];
  b2 = op_b[1][0];
  c2 = op_b[0][1];
  d2 = op_b[1][1];

  T cpd = c1 + d1;
  T cma = c2 - a2;
  T cmamd = cma - d2;
  T ata = a1 * a2;

  T u = (c1 - a1) * (c2 - d2);
  T v = cpd * cma;
  T w = ata - (cpd - a1) * cmamd;
  T wpu = w + u;

  op_d[0][0] = op_c[0][0] + ata + b1 * b2;
  op_d[0][1] = op_c[0][1] + w + v + (a1 + b1 - cpd) * d2;
  op_d[1][0] = op_c[1][0] + wpu + d1 * (b2 + cmamd);
  op_d[1][1] = op_c[1][1] + wpu + v;
}
}  // namespace operators
}  // namespace hw
}  // namespace ama
