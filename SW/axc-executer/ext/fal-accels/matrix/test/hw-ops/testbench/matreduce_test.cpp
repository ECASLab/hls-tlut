/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 * Based on the work by: Eduardo Salazar Villalobos
 * <eduarsalazar@estudiantec.cr>
 */

#include <cstdlib>
#include <iostream>

#include "linear.hpp"
#include "matreduce_top_accel.hpp"
#include "utils/measure.hpp"

int main(int argc, char **argv) {
  float in_mat_a[ROWS][COLS];
  float sw_result[1][1] = {{0.f}};
  ExactType hw_in_mat_a[ROWS][COLS];
  ExactType hw_result;

  int err_cnt = 0;
  constexpr float limit_factor = 0.5f;
  constexpr float error_tolerance = 0.20f;
  ama::utils::StatsMeter meter{};

  /* Generate the matrices */
  srand(SEED);
  for (int i = 0; i < ROWS; ++i) {
    for (int j = 0; j < COLS; ++j) {
      /* Create the input matrices */
      in_mat_a[i][j] = limit_factor * (float)std::rand() / (float)RAND_MAX;
      in_mat_a[i][j] *= (j % 2 == 0 ? -1 : 1);
      /* Copy to the hardware types */
      hw_in_mat_a[i][j] = in_mat_a[i][j];
      /* Compute the SW result */
      sw_result[0][0] += in_mat_a[i][j];
    }
  }

  /* Execute accel */
  matreduce_top_accel(hw_in_mat_a, hw_result);

  float hw_result_f[1][1] = {{static_cast<float>(hw_result)}};
  meter.Register(sw_result[0][0], hw_result_f[0][0], 2.f);

  ama::utils::compare_results<float, 1, 1>(hw_result_f, sw_result, err_cnt,
                                           error_tolerance);

  return 0;
}
