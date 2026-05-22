/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include "testbench.hpp"

/**
 * @brief Matrix reduce accelerator top
 *
 * This function performs a reduce operation using the MatrixReduce operator
 *
 * @param a Matrix of MxM dimensions
 * @param res Scalar with the result
 */
void matreduce_top_accel(ExactType a[ROWS][COLS], ExactType &res);
