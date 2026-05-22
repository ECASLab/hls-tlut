/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>
#include <opencv2/opencv.hpp>
#include "convolution.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

#define K 7
#define mRows 512
#define nCols 512

/**
 * @brief Matrix spatial convolution and padding
 * It performs the convolution of two matrices and padding from header files.
 * @param selection the switch case int selection
 * @param a Matrix use as input or kernel depend of the function
 * @param b Matrix padd for a kernel size k and input already padd depend of the
 * function
 * @param c Matrix with the result of the convolution for specific function
 */

void operations_top_sw(int selection, const float a[mRows][nCols],
                       float b[mRows + (K-1)][nCols + (K-1)], 
                       float c[mRows + (K-1)][nCols + (K-1)], 
                       float d[K][K]){
  switch (selection) {
    case 0:
      ama::sw::padding<float, mRows, nCols, K>(a, b);
      break;
    case 1:
      ama::sw::conv<float, mRows, nCols, K>(d, b, c);
      break;
  }
}

/**
 * @brief Matrix use as test for the functions
 * It performs the convolution of two matrices.
 * @param a Matrix use as input
 * @param b Matrix empty matrix use for padding
 * @param c Matrix use as kernel
 * @param d convolute matrix
 * @param img Original image for OpenCV
 * @param Aimg Output image with the convolution applied
 */

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Cannot open the file. No enough args" << std::endl;
    return -1;
  }
  cv::Mat img = cv::imread(argv[1], 0);
  int M = img.rows;
  int N = img.cols;
  float a[mRows][nCols] = {0};

  for (int i{0}; i < M; ++i) {
    for (int j{0}; j < N; ++j) {
      a[i][j] = img.at<uint8_t>(i, j);
    }
  }
  float b[mRows + (K - 1)][nCols + (K - 1)] = {0};
  float c[K][K];
  #if K == 5
    ama::utils::add_kernel5<float, K>(c);
  #elif K == 7
    ama::utils::add_kernel7<float, K>(c);
  #elif K == 3
    ama::utils::add_kernel3<float, K>(c);
  #else
  #error "Cannot proceed with the given kernel"
  #endif


  float d[mRows + (K - 1)][nCols + (K - 1)] = {0};

operations_top_sw(0, a, b, d, c);
operations_top_sw(1, a, b, d, c);

  cv::Mat Aimg(mRows + (K - 1), nCols + (K - 1), CV_8U, a);
  for (int i{0}; i < (mRows + (K - 1)); ++i) {
    for (int j{0}; j < (nCols + (K - 1)); ++j) {
      Aimg.at<uint8_t>(i, j) = d[i][j];
    }
  }

cv::imshow("Aimg", Aimg);
cv::imshow("img", img);
cv::waitKey(0);
  return 0;
}
