/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>
#include <opencv2/opencv.hpp>

#include "opencv_utils.hpp"
#include "single_core.hpp"
#include "utils/image_stats.hpp"
#include "utils/printmatrix.hpp"

int main(int argc, char **argv) {
  const std::string kDefaultInputImage{"misc/lenna"};
  const std::string kDefaultOutputImage{"misc/lenna-output"};
  const bool kEnablePrinting = false;
  int ret = 0;
  DataType output_batch[kOutputSize][kOutputSize];
  DataType input_batch[kWindowSize][kWindowSize];
  DataType kernel[Q_K][Q_K];

  cv::Mat input_img, output_hw, output_sw, kernel_sw;

  /* Get the input image */
  if (argc < 2) {
    input_img = cv::imread(kDefaultInputImage, 0);
  } else {
    input_img = cv::imread(argv[1], 0);
  }

  /* Check it */
  if (input_img.empty()) {
    std::cerr << "Cannot open the image" << std::endl;
    return -1;
  }

  std::cout << "Image: " << argv[1] << std::endl;

  /* Copy the kernel */
  kernel_sw = get_filter(Q_K);
  CopyMatToArray<double, false>(kernel_sw, kernel[0], cv::Rect{0, 0, Q_K, Q_K});
  std::cout << "Kernel: " << std::endl;
  ama::utils::print_matrix<DataType, Q_K, Q_K>(kernel);

  /* Create a padded image */
  const int offset = Q_K / 2;
  cv::Mat input_img_pad = cv::Mat::zeros(
      input_img.rows + Q_K - 1, input_img.cols + Q_K - 1, input_img.type());
  cv::Mat roi_input_img_pad =
      input_img_pad(cv::Rect{offset, offset, input_img.cols, input_img.rows});
  input_img.copyTo(roi_input_img_pad);

  output_hw = cv::Mat::zeros(input_img.size(), input_img.type());
  std::cout << "Image size: " << output_hw.size() << std::endl;

  /* Send matrix */
  const int step_x = kOutputSize;
  const int step_y = kOutputSize;

  for (int i{0}; i < input_img.rows; i += step_y) {
    for (int j{0}; j < input_img.cols; j += step_x) {
      /* Get the ROI and load the data */
      cv::Rect roi_in{j, i, kWindowSize, kWindowSize};
      CopyMatToArray(input_img_pad, input_batch[0], roi_in);

      /* Process matrix */
      if (kEnablePrinting) {
        ama::utils::print_matrix<DataType, kWindowSize, kWindowSize>(
            input_batch);
      }
      single_core_top_accel(input_batch, kernel, output_batch);
      if (kEnablePrinting) {
        ama::utils::print_matrix<DataType, kOutputSize, kOutputSize>(
            output_batch);
      }

      /* Get results */
      cv::Rect roi_out{j, i, step_x, step_y};
      CopyMatFromArray(output_batch[0], output_hw, roi_out);
    }
  }

  /* Compute the SW version */
  CvFilter(input_img, kernel_sw, output_sw);

  /* Print output matrices */
  if (argc < 3) {
    cv::imwrite(kDefaultOutputImage, output_hw);
    cv::imwrite(kDefaultOutputImage + "-sw.png", output_sw);
  } else {
    cv::imwrite(argv[2], output_hw);
    cv::imwrite(std::string{argv[2]} + "-sw.png", output_sw);
  }

  /* Extract frame differences and error */
  cv::Mat output_float_sw, output_float_hw;
  output_sw.convertTo(output_float_sw, CV_64F);
  output_hw.convertTo(output_float_hw, CV_64F);

  cv::Mat abs_difference = (cv::abs(output_float_sw - output_float_hw) / 256.);
  auto mean_std = ama::utils::mean_std(abs_difference);

  std::cout << "Image RMSE: "
            << std::sqrt(ama::utils::mse(output_float_sw, output_float_hw)) /
                   256.
            << std::endl;
  std::cout << "Image PSNR: "
            << ama::utils::psnr(output_float_sw, output_float_hw) << std::endl;
  std::cout << "Image Mean: " << mean_std.first << std::endl;
  std::cout << "Image Std: " << mean_std.second << std::endl;
  std::cout << "Image SSIM: "
            << ama::utils::ssim(output_float_sw, output_float_hw) << std::endl;
  /* 30% */
  std::cout << "Hist 500 bins 30%: "
            << ama::utils::histogram(abs_difference, 500, 0.30) << std::endl;

  /* Extract kernel differences */
  cv::Mat kernel_hw{kernel_sw.size(), kernel_sw.type()};
  CopyMatFromArray<double, false>(kernel[0], kernel_hw,
                                  cv::Rect{0, 0, Q_K, Q_K});

  abs_difference = cv::abs(kernel_sw - kernel_hw);
  mean_std = ama::utils::mean_std(abs_difference);

  std::cout << "Kernel RMSE: "
            << std::sqrt(ama::utils::mse(kernel_sw, kernel_hw)) << std::endl;
  std::cout << "Kernel PSNR: " << ama::utils::psnr(kernel_sw, kernel_hw)
            << std::endl;
  std::cout << "Kernel Mean: " << mean_std.first << std::endl;
  std::cout << "Kernel Std: " << mean_std.second << std::endl;

  /* Co-sim patch */
  single_core_top_accel(input_batch, kernel, output_batch);

  return ret;
}
