/*
 * Copyright 2022-2023
 * Author: Fabricio Elizondo Fernandez <faelizondo@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include "MatrixOperator.hpp"
#include "cores/arithmetic-exact.hpp"

namespace ama {
namespace hw {
namespace operators {

using namespace ama::hw;

/**
 * Map template Operator
 * It is responsible for transforming an input matrix, applying a unary
 * operation to each element of it.
 * @tparam T datatype to work with
 * @tparam M rows of a matrix
 * @tparam N columns of a matrix
 * @tparam UOP Unary Operation as a functor. Defaults to Passthru
 */
template <typename T, int M, int N, class UOP = arithmetic::exact::PassThru<T>>
class MatrixMap : public MatrixOperator<T, M, N> {
 public:
  /**
   * Execute the mapping process on a given matrix
   * It uses the functor operator received as a template parameter
   * @param input_matrix input matrix
   * @param result_matrix output matrix after the transformation
   */
  virtual void Execute(const T input_matrix[M][N], T result_matrix[M][N]);

 private:
  UOP unary_op_{};
};

template <typename T, int M, int N, class OP>
void MatrixMap<T, M, N, OP>::Execute(const T input_matrix[M][N],
                                     T result_matrix[M][N]) {
rows_loop:
  for (int i = 0; i < M; ++i) {
  cols_loop:
    for (int j = 0; j < N; ++j) {
      result_matrix[i][j] = unary_op_(input_matrix[i][j]);
    }
  }
}

} /* namespace operators */
} /* namespace hw */
} /* namespace ama */
