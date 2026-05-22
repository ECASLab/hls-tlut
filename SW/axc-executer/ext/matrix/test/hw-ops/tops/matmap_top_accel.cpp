/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

/**
 * @example matmap_top_accel.cpp
 * Matrix map example
 */

#include "matmap_top_accel.hpp"

#include "linear.hpp"

void matmap_top_accel(ExactType a[ROWS][COLS], ExactType res[ROWS][COLS]) {
  /* Create new engine */
  ama::hw::operators::MatrixMap<ExactType, ROWS, COLS, TestOperation<ExactType>>
      map{};
  map.Execute(a, res);
}
