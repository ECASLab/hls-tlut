/*
 * Copyright 2023
 * Author: Alex Chacon-Rodriguez <alexchr10@gmail.com>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <ctime>
#include <iostream>

#include "linear.hpp"
#include "matstrassen_top_accel.hpp"
#include "utils/measure.hpp"

int main(int argc, char **argv) {
  float in_mat_a[ROWS][COLS];
  float in_mat_b[COLS][ROWS];
  float sw_result[ROWS][ROWS];
  ExactType hw_in_mat_a[ROWS][COLS];
  ExactType hw_in_mat_b[COLS][ROWS];
  ExactType hw_result[ROWS][ROWS];
  int err_cnt = 0;
  const float limit_factor = float((1 << (WL - 1))) / float((1 << WL));
  ama::utils::StatsMeter meter{};
  srand(SEED);

  for (int c = 0; c < 5000; ++c) {
    for (int i = 0; i < ROWS; ++i) {
      for (int j = 0; j < COLS; ++j) {
        in_mat_a[i][j] = limit_factor * (float)std::rand() / (float)RAND_MAX;
        in_mat_b[j][i] = limit_factor * (float)std::rand() / (float)RAND_MAX;
        in_mat_a[i][j] *= (j % 2 == 0 ? -1 : 1);
        in_mat_b[j][i] *= (j % 3 == 0 && c % 2 == 0 ? -1 : 1);
        hw_in_mat_a[i][j] = (in_mat_a[i][j]);
        hw_in_mat_b[j][i] = (in_mat_b[j][i]);
      }
    }

    ama::sw::matmul<float, ROWS, COLS>(in_mat_a, in_mat_b, sw_result);
    matstrassen_top_accel(hw_in_mat_a, hw_in_mat_b, hw_result);

    float hw_result_f[ROWS][ROWS];
    for (int i = 0; i < ROWS; ++i) {
      for (int j = 0; j < ROWS; ++j) {
        hw_result_f[i][j] = static_cast<float>(hw_result[i][j]);
        meter.Register(sw_result[i][j], hw_result_f[i][j], 1.);
      }
    }

    ama::utils::print_matrices<float, ROWS, ROWS>(hw_result_f);
    ama::utils::print_matrices<float, ROWS, COLS>(sw_result);
  }

  return 0;
}