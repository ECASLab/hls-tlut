/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>

#include "utils/printmatrix.hpp"
#include "winograd.hpp"

#if Q_K == 3
static DataType kKernel[kKernelSize][kKernelSize] = {
    {0.0625, 0.125, 0.0625}, {0.125, 0.25, 0.125}, {0.0625, 0.125, 0.0625}};
static DataType kOnes[kWindowSize][kWindowSize] = {{0.5, 0.5, 0.5, 0.5},
                                                   {0.5, 0.1, 0.1, 0.5},
                                                   {0.5, 0.1, 0.1, 0.5},
                                                   {0.5, 0.5, 0.5, 0.5}};
#elif Q_K == 5
static DataType kKernel[kKernelSize][kKernelSize] = {
    {0.04, 0.04, 0.04, 0.04, 0.04},
    {0.04, 0.04, 0.04, 0.04, 0.04},
    {0.04, 0.04, 0.04, 0.04, 0.04},
    {0.04, 0.04, 0.04, 0.04, 0.04},
    {0.04, 0.04, 0.04, 0.04, 0.04}};
static DataType kOnes[kWindowSize][kWindowSize] = {
    {0.5, 0.5, 0.5, 0.5, 0.5, 0.5}, {0.5, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.5}, {0.5, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.5}, {0.5, 0.5, 0.5, 0.5, 0.5, 0.5}};
#elif Q_K == 7
static DataType kKernel[kKernelSize][Q_K] = {
    {0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204},
    {0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204},
    {0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204},
    {0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204},
    {0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204},
    {0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204},
    {0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204, 0.0204}};
static DataType kOnes[kWindowSize][kWindowSize] = {
    {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.5},
    {0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5}};
#else
#error "Unsuitable kernel size for the testbench"
#endif

int main(int, char**) {
  int ret = 0;
  DataType output[kOutputSize][kOutputSize];

  /* Print input matrices */
  std::cout << "Input matrix: " << std::endl;
  ama::utils::print_matrix<DataType, kWindowSize, kWindowSize>(kOnes);
  std::cout << "Kernel matrix: " << std::endl;
  ama::utils::print_matrix<DataType, kKernelSize, kKernelSize>(kKernel);

  winograd_top_accel(kOnes, kKernel, output);

  /* Print output matrices */
  std::cout << "Output matrix: " << std::endl;
  ama::utils::print_matrix<DataType, kOutputSize, kOutputSize>(output);

  return ret;
}
