/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <opencv2/opencv.hpp>

#define ANCHOR cv::Point(-1, -1)
#define BORDER_TYPE cv::BORDER_CONSTANT
#define DELTA 0
#define IMAGE_DEPTH CV_64F

static cv::Mat get_filter(const int kernel_size) {
  cv::Mat filter;
  double sigma;

  sigma = (kernel_size + 2) / 6;

  filter = cv::getGaussianKernel(kernel_size, sigma, IMAGE_DEPTH);
  return filter * filter.t();
}

template <typename T = uint8_t, bool norm = true, typename TO>
static void CopyMatToArray(const cv::Mat &input, TO *output,
                           const cv::Rect &roi) {
  for (int i{0}; i < roi.height; ++i) {
    for (int j{0}; j < roi.width; ++j) {
      float val = input.at<T>(i + roi.y, j + roi.x);
      output[i * roi.width + j] = norm ? (val / 256.f - 0.5f) : val;
    }
  }
}

template <typename T = uint8_t, bool norm = true, typename TO>
static void CopyMatFromArray(const TO *input, cv::Mat &output,
                             const cv::Rect &roi) {
  for (int i{0}; i < roi.height; ++i) {
    for (int j{0}; j < roi.width; ++j) {
      float val = input[i * roi.width + j];
      output.at<T>(roi.y + i, j + roi.x) = norm ? ((val + 0.5f) * 256.f) : val;
    }
  }
}

static void CvFilter(const cv::Mat &input, const cv::Mat &kernel,
                     cv::Mat &output) {
  cv::filter2D(input, output, IMAGE_DEPTH, kernel, ANCHOR, DELTA, BORDER_TYPE);
}
