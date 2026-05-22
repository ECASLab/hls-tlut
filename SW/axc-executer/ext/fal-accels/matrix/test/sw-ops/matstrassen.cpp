/*
 * Copyright 2023
 * Author: Alex Chacón Rodríguez <alexchr10@gmail.com>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>
#include "linear.hpp"
#include "testbench.hpp"

/* Glob params */
static constexpr int kSize = 8;

/**
 * Matrix operator for split matrix in submatrices
 * @tparam T datatype
 * @tparam M rows of a matrix
 * @tparam N columns of a matrix
 * @tparam HM half of M value
 * @tparam HN half of N value
 * @param x matrix received to split
 * @param a first submatrix after the split
 * @param b second submatrix after the split
 * @param c third submatrix after the split
 * @param d fourth submatrix after the split
 */

template <typename T, int M, int N, int HM = M / 2, int HN = N / 2>
void MatrixSplit(const T x[M][N], T a[HM][HN], T b[HM][HN], T c[HM][HN],
                 T d[HM][HN]) {
  for (int i = 0; i < HM; ++i) {
    for (int j = 0; j < HM; ++j) {
      a[i][j] = x[i][j];
      b[i][j] = x[i][HM + j];
      c[i][j] = x[HM + i][j];
      d[i][j] = x[HM + i][HM + j];
    }
  }
}
/**
 * Matrix operator for merge several submatrices in a one matrix
 * @tparam T datatype
 * @tparam M rows of a matrix
 * @tparam N columns of a matrix
 * @tparam DM double of M value
 * @tparam DN double of N value
 * @param a first submatrix after the split
 * @param b second submatrix after the split
 * @param c third submatrix after the split
 * @param d fourth submatrix after the split
 * @param merged_matrix merged matrix after the merge of submatrices
 */
template <typename T, int M, int N, int DM = 2 * M, int DN = 2 * N>
void MatrixMerge(const T a[M][N], const T b[M][N], const T c[M][N],
                 const T d[M][N], T merged_matrix[DM][DN]) {
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < M; ++j) {
      merged_matrix[i][j] = a[i][j];
      merged_matrix[i][M + j] = b[i][j];
      merged_matrix[M + i][j] = c[i][j];
      merged_matrix[M + i][M + j] = d[i][j];
    }
  }
}
/**
 * Matrix operator for use a Strassen multiply in 2x2 matrices
 * @tparam T datatype
 * @tparam M rows of a matrix
 * @tparam N columns of a matrix
 * @param A first matrix received to multiply with Strassen
 * @param B second matrix received to multipĺy with Strassen
 * @param result result of the Strassen multiply
 */

template <typename T, int M, int N>
void Strassen2x2(const T A[M][N], const T B[M][N], T result[M][N]) {
  constexpr int kHalfM = M / 2;
  constexpr int kHalfN = N / 2;
  T a[kHalfM][kHalfN], b[kHalfM][kHalfN], c[kHalfM][kHalfN],
      d[kHalfM][kHalfN];
  T e[kHalfM][kHalfN], f[kHalfM][kHalfN], g[kHalfM][kHalfN],
      h[kHalfM][kHalfN];

  MatrixSplit<T, M, N>(A, a, b, c, d);
  MatrixSplit<T, M, N>(B, e, f, g, h);

  T p1 = a[0][0] * (f[0][0] - h[0][0]);
  T p2 = (a[0][0] + b[0][0]) * h[0][0];
  T p3 = (c[0][0] + d[0][0]) * e[0][0];
  T p4 = d[0][0] * (g[0][0] - e[0][0]);
  T p5 = (a[0][0] + d[0][0]) * (e[0][0] + h[0][0]);
  T p6 = (b[0][0] - d[0][0]) * (g[0][0] + h[0][0]);
  T p7 = (a[0][0] - c[0][0]) * (e[0][0] + f[0][0]);

  T res1[kHalfM][kHalfM] = {{p5 + p4 - p2 + p6}};
  T res2[kHalfM][kHalfM] = {{p1 + p2}};
  T res3[kHalfM][kHalfM] = {{p3 + p4}};
  T res4[kHalfM][kHalfM] = {{p1 + p5 - p3 - p7}};

  MatrixMerge<T, kHalfM, kHalfM>(res1, res2, res3, res4, result);
}

int main() {
  using Datatype = int;

  Datatype A[kSize][kSize] = {0};
  Datatype B[kSize][kSize] = {0};
  Datatype out_matrix[kSize][kSize] = {0};

  /* Random matrices generation */
  for (int i = 0; i < kSize; ++i) {
    for (int j = 0; j < kSize; ++j) {
      A[i][j] = 10 * (ExactType)rand() / (ExactType)RAND_MAX;
      B[j][i] = 10 * (ExactType)rand() / (ExactType)RAND_MAX;
      A[i][j] *= j % 2 ? -1 : 1;
      B[j][i] *= j % 3 ? -1 : 1;
    }
  }

  /* Rows 2 by 2 steps */
  for (int yo = 0; yo < kSize; yo += 2) {
  /* Columns 2 by 2 steps */
    for (int xo = 0; xo < kSize; xo += 2) {
    /* Columns for A and rows for B 2 by 2 steps */
      for (int ko = 0; ko < 8; ko += 2) {

      int subMatrix_A[2][2] = {0};
      int subMatrix_B[2][2] = {0};
      int result[2][2] = {0};

      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
        /* submatrices 2x2 in A */
        subMatrix_A[i][j] = A[i + yo][j + ko];
        /* submatrices 2x2 in B */
        subMatrix_B[i][j] = B[i + ko][j + xo];
      }
      }
      /* Strassen 2x2 submatrixA & submatrixB */
      Strassen2x2<Datatype, 2, 2>(subMatrix_A, subMatrix_B, result);

      /* Cumulative for out_matrix */
      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
        /* Acumulamos en la matriz de salida */
        out_matrix[i + yo][j + xo] += result[i][j];
      }
    }
  }
  }
  }
  ama::utils::print_matrices<Datatype, kSize, kSize>(out_matrix); 
  return 0;
}