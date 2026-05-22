/*
 * Copyright 2023
 * Author: Alex Chacon-Rodriguez <alexchr10@gmail.com>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once
#include "MatrixOperator.hpp"

namespace ama {
namespace hw {
namespace operators {

using namespace ama::hw;

/**
 * Matrix operator for matrix multiplication with Strassen algorithm
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
class MatrixMultiplyStrassen : public MatrixOperator<T, M, N, ADD, MULT, NL> {
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
      T op_c[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
            [MatrixOperator<T, M, N, ADD, MULT, NL>::rows]);

 private:
  ADD add_{};
  MULT mult_{};
  NL non_linearity_{};
  void MatrixSplit(const T x[M][N], T a[M / 2][N / 2], T b[M / 2][N / 2],
                   T c[M / 2][N / 2], T d[M / 2][N / 2]);
  void MatrixMerge(const T a[M / 2][N / 2], const T b[M / 2][N / 2],
                   const T c[M / 2][N / 2], const T d[M / 2][N / 2],
                   T merged_matrix[M][N]);
  void Strassen2x2(const T A[M][N], const T B[M][N], T result[M][N]);
};

template <typename T, int M, int N, class ADD, class MULT, class NL>
inline void MatrixMultiplyStrassen<T, M, N, ADD, MULT, NL>::Execute(
    const T op_a[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
                [MatrixOperator<T, M, N, ADD, MULT, NL>::columns],
    const T op_b[MatrixOperator<T, M, N, ADD, MULT, NL>::columns]
                [MatrixOperator<T, M, N, ADD, MULT, NL>::rows],
    T op_c[MatrixOperator<T, M, N, ADD, MULT, NL>::rows]
          [MatrixOperator<T, M, N, ADD, MULT, NL>::rows]) {
#pragma HLS inline recursive
  static_assert(M == 2 && N == 2, "Strassen supports only 2x2 matrices");
  Strassen2x2(op_a, op_b, op_c);
}

template <typename T, int M, int N, class ADD, class MULT, class NL>
inline void MatrixMultiplyStrassen<T, M, N, ADD, MULT, NL>::MatrixSplit(
    const T x[M][N], T a[M / 2][N / 2], T b[M / 2][N / 2], T c[M / 2][N / 2],
    T d[M / 2][N / 2]) {
  constexpr int HM = M / 2;
  constexpr int HN = N / 2;
  for (int i = 0; i < HM; i++) {
    for (int j = 0; j < HN; j++) {
      a[i][j] = x[i][j];
      b[i][j] = x[i][HM + j];
      c[i][j] = x[HM + i][j];
      d[i][j] = x[HM + i][HM + j];
    }
  }
}

template <typename T, int M, int N, class ADD, class MULT, class NL>
inline void MatrixMultiplyStrassen<T, M, N, ADD, MULT, NL>::MatrixMerge(
    const T a[M / 2][N / 2], const T b[M / 2][N / 2], const T c[M / 2][N / 2],
    const T d[M / 2][N / 2], T merged_matrix[M][N]) {
  constexpr int HM = M / 2;
  constexpr int HN = N / 2;
  for (int i = 0; i < HM; ++i) {
    for (int j = 0; j < HN; ++j) {
      merged_matrix[i][j] = a[i][j];
      merged_matrix[i][HN + j] = b[i][j];
      merged_matrix[HM + i][j] = c[i][j];
      merged_matrix[HM + i][HN + j] = d[i][j];
    }
  }
}

template <typename T, int M, int N, class ADD, class MULT, class NL>
inline void MatrixMultiplyStrassen<T, M, N, ADD, MULT, NL>::Strassen2x2(
    const T A[M][N], const T B[M][N], T result[M][N]) {
  constexpr int kHalfM = M / 2;
  constexpr int kHalfN = N / 2;
  T a[kHalfM][kHalfN], b[kHalfM][kHalfN], c[kHalfM][kHalfN], d[kHalfM][kHalfN];
  T e[kHalfM][kHalfN], f[kHalfM][kHalfN], g[kHalfM][kHalfN], h[kHalfM][kHalfN];

  MatrixSplit(A, a, b, c, d);
  MatrixSplit(B, e, f, g, h);

  T p1 = mult_(a[0][0], add_(f[0][0], -h[0][0]));
  T p2 = mult_(add_(a[0][0], b[0][0]), h[0][0]);
  T p3 = mult_(add_(c[0][0], d[0][0]), e[0][0]);
  T p4 = mult_(d[0][0], add_(g[0][0], -e[0][0]));
  T p5 = mult_(add_(a[0][0], d[0][0]), add_(e[0][0], h[0][0]));
  T p6 = mult_(add_(b[0][0], -d[0][0]), add_(g[0][0], h[0][0]));
  T p7 = mult_(add_(a[0][0], -c[0][0]), add_(e[0][0], f[0][0]));

  /* p5 + p4 - p2 + p6 */
  T res1[kHalfM][kHalfM] = {{add_(add_(add_(p5, p4), -p2), p6)}};
  /* p1 + p2 */
  T res2[kHalfM][kHalfM] = {{add_(p1, p2)}};
  /* p3 + p4 */
  T res3[kHalfM][kHalfM] = {{add_(p3, p4)}};
  /* p1 + p5 - p3 - p7 */
  T res4[kHalfM][kHalfM] = {{add_(add_(add_(p1, p5), -p3), -p7)}};

  MatrixMerge(res1, res2, res3, res4, result);
}
}  // namespace operators
}  // namespace hw
}  // namespace ama
