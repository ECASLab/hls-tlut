/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <opencv2/opencv.hpp>
#include <tuple>

namespace ama {
namespace utils {

/**
 * Mean Square Error for OpenCV Matrices
 * @param golden golden data
 * @param sample experimental data
 * @return the MSE in double
 */
double mse(const cv::Mat &golden, const cv::Mat &sample) {
  assert(!golden.empty());
  assert(!sample.empty());

  /* Take the first one as golden data */
  cv::Mat golden_float, sample_float;
  golden.convertTo(golden_float, CV_64F);
  sample.convertTo(sample_float, CV_64F);

  /* Compute differences */
  cv::Mat diff = golden_float - sample_float;
  cv::Mat squared = diff.mul(diff);

  /* Full reduce */
  cv::Mat col_sum, singleton;
  cv::reduce(squared, col_sum, 0, 0, CV_64F);
  cv::reduce(col_sum, singleton, 1, 0, CV_64F);

  return singleton.at<double>(0, 0) / (golden.rows * golden.cols);
}

/**
 * Compute the histogram given a matrix, a maximum and a number of bins
 * @param input_matrix matrix with the data to count
 * @param bins number of bins
 * @param max maximum of the values to count
 * @return histogram matrix 1-D
 */
cv::Mat histogram(const cv::Mat &input_matrix, const int bins, double max) {
  cv::Mat hist;

  const int channels[] = {0};
  const int hbins[] = {bins};
  const float range[] = {0.f, static_cast<float>(max)};
  const float *hranges[] = {range};

  cv::Mat matrix_float;
  input_matrix.convertTo(matrix_float, CV_32F);

  cv::calcHist(&matrix_float, 1, {0}, cv::Mat(), hist, 1, hbins, hranges, true,
               false);

  return hist;
}

/**
 * Computes the mean and the standard deviation
 * @param matrix matrix with the data
 * @return pair with the mean and the std respectively
 */
std::pair<double, double> mean_std(const cv::Mat matrix) {
  assert(!matrix.empty());

  /* Take the first one as golden data */
  cv::Mat matrix_float;
  matrix.convertTo(matrix_float, CV_64F);

  /* Full reduce */
  cv::Mat mean_mat, std_mat;
  cv::meanStdDev(matrix_float, mean_mat, std_mat);

  return std::pair<double, double>(mean_mat.at<double>(0, 0),
                                   std_mat.at<double>(0, 0));
}

/**
 * Computes the minimum and the maximum
 * @param matrix matrix with the data
 * @return pair with the min and the max respectively
 */
std::pair<double, double> min_max(const cv::Mat &matrix) {
  /* Find the max of the golden */
  double min_val, max_val;
  cv::Point min_loc, max_loc;
  cv::minMaxLoc(matrix, &min_val, &max_val, &min_loc, &max_loc);
  return std::pair<double, double>{min_val, max_val};
}

/**
 * Computes Peak Signal-to-Noise Ratio
 * @param golden golden data
 * @param sample experimental data
 * @return the PSNR in double
 */
double psnr(const cv::Mat golden, const cv::Mat sample) {
  assert(!golden.empty());
  assert(!sample.empty());

  /* Compute the MSE */
  double mse_ = mse(golden, sample);

  /* Compute the max */
  cv::Mat golden_float;
  golden.convertTo(golden_float, CV_64F);

  /* Find the max of the golden */
  auto min_max_ = min_max(golden_float);
  double max_ = min_max_.second;

  return (20 * std::log10(max_) - 10 * std::log10(mse_));
}

/**
 * Computes the covariance
 * @param x_p matrix 1
 * @param y_p matrix 2
 * @return the covariance in double
 */
double covariance(const cv::Mat x_p, const cv::Mat y_p) {
  assert(!x_p.empty());
  assert(!y_p.empty());

  cv::Mat x, y;

  x_p.convertTo(x, CV_64F);
  y_p.convertTo(y, CV_64F);

  /* Compute the means */
  auto mean_std_x = mean_std(x);
  double mean_x = mean_std_x.first;
  auto mean_std_y = mean_std(y);
  double mean_y = mean_std_y.first;

  /* Differences and products (X - E[X])(Y - E[Y]) */
  cv::Mat xvar = x - mean_x;
  cv::Mat yvar = y - mean_y;
  cv::Mat cvar = xvar.mul(yvar);

  /* Expectation */
  cv::Mat col_sum, singleton;
  cv::reduce(cvar, col_sum, 0, 0, CV_64F);
  cv::reduce(col_sum, singleton, 1, 0, CV_64F);

  return singleton.at<double>(0, 0) / (x.rows * x.cols);
}

/**
 * Computes Structural Similarity Index
 * @param x_p matrix 1
 * @param y_p matrix 2
 * @return the SSIM in double
 */
double ssim(const cv::Mat x_p, const cv::Mat y_p) {
  assert(!x_p.empty());
  assert(!y_p.empty());

  cv::Mat x, y;

  x_p.convertTo(x, CV_64F);
  y_p.convertTo(y, CV_64F);

  auto mean_std_x = mean_std(x);
  auto mean_std_y = mean_std(y);

  double mean_x = mean_std_x.first, mean_y = mean_std_y.first;
  double std_x = mean_std_x.second, std_y = mean_std_y.second;
  double covxy = covariance(x, y);

  double c1 = 0.01 * 255, c2 = 0.03 * 255;
  c1 *= c1;
  c2 *= c2;

  double num = (2 * mean_x * mean_y + c1) * (2 * covxy + c2);
  double den = (mean_x * mean_x + mean_y * mean_y + c1) *
               (std_x * std_x + std_y * std_y + c2);

  return num / den;
}

}  // namespace utils
}  // namespace ama
