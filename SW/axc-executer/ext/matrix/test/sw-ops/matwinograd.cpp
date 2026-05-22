/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#include <iostream>

#include "linear.hpp"
#include "testbench.hpp"

/* Glob params */
static constexpr int kSize = 8;

/**
 * Matrix operator for use a Winograd multiply in 2x2 matrices
 * @tparam T datatype
 * @tparam T datatype
 * @tparam M rows of a matrix
 * @tparam N columns of a matrix
 * @param A first matrix received to multiply with Winograd
 * @param B second matrix received to multipĺy with Winograd
 * @param result result of the Winograd multiply
 */
template <typename T, int M, int N>
void Winograd2x2(const T A[M][N], const T B[M][N], T result[M][N]) {
  T a1, b1, c1, d1;
  T a2, b2, c2, d2;

  a1 = A[0][0];
  b1 = A[0][1];
  c1 = A[1][0];
  d1 = A[1][1];

  a2 = B[0][0];
  b2 = B[1][0];
  c2 = B[0][1];
  d2 = B[1][1];

  T cpd = c1 + d1;
  T cma = c2 - a2;
  T cmamd = cma - d2;
  T ata = a1 * a2;

  T u = (c1 - a1) * (c2 - d2);
  T v = (cpd)*cma;
  T w = ata - (cpd - a1) * cmamd;
  T wpu = w + u;

  result[0][0] = ata + b1 * b2;
  result[0][1] = w + v + (a1 + b1 - cpd) * d2;
  result[1][0] = wpu + d1 * (b2 + cmamd);
  result[1][1] = wpu + v;
}

int main() {
  using Datatype = int;

  Datatype A[kSize][kSize] = {0};
  Datatype B[kSize][kSize] = {0};
  Datatype out_matrix[kSize][kSize] = {0};

  int Af[kSize][kSize] = {0};
  int Bf[kSize][kSize] = {0};
  int out_matrix_f[kSize][kSize] = {0};

  /* Random matrices generation */
  for (int i = 0; i < kSize; ++i) {
    for (int j = 0; j < kSize; ++j) {
      Af[i][j] = 10 * (ExactType)rand() / (ExactType)RAND_MAX;
      Bf[j][i] = 10 * (ExactType)rand() / (ExactType)RAND_MAX;
      Af[i][j] *= j % 2 ? -1 : 1;
      Bf[j][i] *= j % 3 ? -1 : 1;
      A[i][j] = Af[i][j];
      B[j][i] = Bf[j][i];
    }
  }

  /* Rows 2 by 2 steps */
  for (int yo = 0; yo < kSize; yo += 2) {
    /* Columns 2 by 2 steps */
    for (int xo = 0; xo < kSize; xo += 2) {
      /* Columns for A and rows for B 2 by 2 steps */
      for (int ko = 0; ko < 8; ko += 2) {
        Datatype subMatrix_A[2][2] = {0};
        Datatype subMatrix_B[2][2] = {0};
        Datatype result[2][2] = {0};

        for (int i = 0; i < 2; ++i) {
          for (int j = 0; j < 2; ++j) {
            /* submatrices 2x2 in A */
            subMatrix_A[i][j] = A[i + yo][j + ko];
            /* submatrices 2x2 in B */
            subMatrix_B[i][j] = B[i + ko][j + xo];
          }
        }
        /* Winograd 2x2 submatrixA & submatrixB */
        Winograd2x2<Datatype, 2, 2>(subMatrix_A, subMatrix_B, result);

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

  /* Execute on SW */
  ama::sw::matmul<int, kSize, kSize>(Af, Bf, out_matrix_f);

  /* Output results */
  std::cout << "SW - Output" << std::endl;
  ama::utils::print_matrices<int, kSize, kSize>(out_matrix_f);
  std::cout << "Winograd - Output" << std::endl;
  ama::utils::print_matrices<Datatype, kSize, kSize>(out_matrix);
  return 0;
}
