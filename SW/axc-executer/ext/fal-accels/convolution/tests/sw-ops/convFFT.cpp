/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <complex>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <valarray>
#include "convolution.hpp"

#define K 7
#define MRows 512
#define NCols 512

/**
 * @brief Matrix fft convolution for 2D images
 * It performs the convolution of two matrices from header files.
 * @param img image use as input with openCV
 * @param a Matrix use as input complex
 * @param a_img output picture in openCV
 * @param c Matrix use as kernel
 */

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cerr << "Cannot open the file. No enough args" << std::endl;
    return -1;
  }
  cv::Mat img = cv::imread(argv[1], 0);

  float b[MRows][NCols] = {0};
  float a[MRows][NCols] = {0};
  
  for (int i{0}; i < MRows; ++i) {
    for (int j{0}; j < NCols; ++j) {
      a[i][j] = img.at<uint8_t>(i, j);
    }
  }

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

  ama::sw::fft_conv_2D<float, MRows, NCols, K>(a, c);

  cv::Mat a_img(MRows, NCols, CV_8U, a);
  for (int i{0}; i < MRows; ++i) {
    for (int j{0}; j < NCols; ++j) {
      a_img.at<uint8_t>(i, j) = a[i][j].real();
    }
  }

  cv::imshow("a_img", a_img);
  cv::imshow("img", img);
  cv::waitKey(0);
  return 0;
}
