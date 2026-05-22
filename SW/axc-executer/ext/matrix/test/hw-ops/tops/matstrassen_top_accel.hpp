/*
 * Copyright 2023
 * Author: Alex Chacon-Rodriguez <alexchr10@gmail.com>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include "testbench.hpp"

/**
 * @brief MatStrassen top
 * It calls the function to perform
 * @param a Matrix of MxN dimensions
 * @param b Matrix of NxM dimensions
 * @param res Matrix with the result
 */
void matstrassen_top_accel(const ExactType a[ROWS][COLS],
                      const ExactType b[COLS][ROWS],
                      ExactType res[ROWS][ROWS]);
