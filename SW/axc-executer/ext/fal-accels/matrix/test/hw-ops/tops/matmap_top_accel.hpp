/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include "testbench.hpp"

/* Test operation */
template <typename T>
class TestOperation {
 public:
  T operator()(const T element) {
    static const T offset{0.25f};
    return element + offset;
  }
};

/**
 * @brief Matrix map accelerator top
 *
 * This function performs a mapping operation using the MatrixMap operator
 *
 * @param a Matrix of MxM dimensions
 * @param res Matrix with the result
 */
void matmap_top_accel(ExactType a[ROWS][COLS], ExactType res[ROWS][COLS]);
