/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

/**
 * @example matreduce_top_accel.cpp
 * Matrix reduce example
 */

#include "matreduce_top_accel.hpp"

#include "linear.hpp"

void matreduce_top_accel(ExactType a[ROWS][COLS], ExactType &res) {
  /* Create new engine */
  ama::hw::operators::MatrixReduce<ExactType, ROWS, COLS> reduce{};
  res = reduce.Execute(a);
}
