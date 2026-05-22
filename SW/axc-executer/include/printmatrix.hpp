/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <runtime.hpp>

#include <iostream>
#include <string>

/**
 * @brief Auxiliar function to print matrices
 *
 * @tparam T datatype of the matrix
 * @param mat matrix data to print
 * @param width width of the matrix to determine the number of columns
 * @param height height of the matrix to determine the number of rows
 * @param name name of the matrix (just for printing purposes and
 * identification)
 * @return Runtime
 */
template <typename T>
Runtime PrintMatrix2d(const T *mat, const int width, const int height,
                      const std::string &name) {
  if (!mat) {
    return Runtime{-1, "Intended to print a null matrix"};
  }

  std::cout << name << "[" << width << "x" << height << "]: " << std::endl;
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      int index = width * i + j;
      std::cout << mat[index] << " ";
    }
    std::cout << std::endl;
  }
  return Runtime{};
}
