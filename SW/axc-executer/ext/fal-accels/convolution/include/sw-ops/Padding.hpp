/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

namespace ama {
namespace sw {

/**
 * @brief Matrix padding
 * It performs the padding on a matrix for a kernel size k.
 * @param input_image Matrix original matrix
 * @param output_image Matrix with the padding applied
 */

template <typename T, int M, int N, int k>

void padding(const T input_image[M][N],
             T output_image[M + (k - 1)][N + (k - 1)]) {
  const int start = k / 2;
  for (int i{0}; i < M + k - 1; ++i) {
      for (int j{0}; j < N + k - 1; ++j) {
          if (i < start && j < start) {
            output_image[i][j] = input_image[0][0];
          } else if (i < start && j > (N - 1 + start)){
            output_image[i][j] = input_image[0][N - 1];
          } else if (i > (M - 1 + start) && j < start){
            output_image[i][j] = input_image[M - 1][0];
          } else if (i > (M - 1 + start) && j > (N - 1 + start)){
            output_image[i][j] = input_image[M - 1][N - 1];
          } else if (i < start){
            output_image[i][j] = input_image[0][j - start];
          } else if (i > (M - 1 + start)){
            output_image[i][j] = input_image[M - 1][j - start];
          } else if (j < start){
            output_image[i][j] = input_image[i - start][0];
          } else if (j > (N - 1 + start)){
            output_image[i][j] = input_image[i - start][N - 1];
          } else {
            output_image[i][j] = input_image[i - start][j - start];
          }
        }
    }
}
}  // namespace sw
}  // namespace ama
