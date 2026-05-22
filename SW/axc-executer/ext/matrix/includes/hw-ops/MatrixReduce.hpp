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
 * Reduce template operator
 * It is responsible for performing a reduction of an input matrix, by applying
 * a binary operation between the elements of the matrix
 * @tparam T datatype to work with
 * @tparam M rows of a matrix
 * @tparam N columns of a matrix
 * @tparam BOP Binary Operation as a functor. Defaults to the Add exact version
 */
template <typename T, int M, int N, class BOP = arithmetic::exact::Add<T>>
class MatrixReduce : public MatrixOperator<T, M, N> {
 public:
  /**
   * Execute the reduction process on a given matrix
   * It uses the functor operator received as a template parameter
   * @param input_matrix input matrix
   * @return T final result of the reduction process
   */
  T Execute(const T input_matrix[M][N]);

 private:
  BOP binary_op_{};
};

template <typename T, int M, int N, class BOP>
T MatrixReduce<T, M, N, BOP>::Execute(const T input_matrix[M][N]) {
  T res{0.};

rows_reduce:
  for (int i = 0; i < M; ++i) {
  cols_reduce:
    for (int j = 0; j < N; ++j) {
      res = binary_op_(input_matrix[i][j], res);
    }
  }

  return res;
}

} /* namespace operators */
} /* namespace hw */
} /* namespace ama */
