/*
 * Copyright 2023
 * Author: Alex Chacon-Rodriguez <alexchr10@gmail.com>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example matmul_top_accel.cpp
 * Matrix Multiplication example
 */

#include "matstrassen_top_accel.hpp"

#include "linear.hpp"

void matstrassen_top_accel(const ExactType a[ROWS][COLS],
                      const ExactType b[COLS][ROWS],
                      ExactType res[ROWS][ROWS]) {
  auto engine = ama::hw::operators::MatrixMultiplyStrassen<ExactType, ROWS, COLS>{};
  engine.Execute(a, b, res);
}
