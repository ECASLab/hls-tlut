/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>
#include <opencv2/opencv.hpp>

#include "convolver_vector.hpp"
#include "utils/printmatrix.hpp"

#if Q_K == 3
static DataType kKernel[Q_K][Q_K] = {
    {0.0625, 0.125, 0.0625}, {0.125, 0.25, 0.125}, {0.0625, 0.125, 0.0625}};

#elif Q_K == 5
static DataType kKernel[Q_K][Q_K] = {{0.04, 0.04, 0.04, 0.04, 0.04},
                                     {0.04, 0.04, 0.04, 0.04, 0.04},
                                     {0.04, 0.04, 0.04, 0.04, 0.04},
                                     {0.04, 0.04, 0.04, 0.04, 0.04},
                                     {0.04, 0.04, 0.04, 0.04, 0.04}};
#else
#error "Unsuitable kernel size for the testbench"
#endif

void CopyMatToArray(const cv::Mat &input, DataType *output,
                    const cv::Rect &roi) {
  for (int i{0}; i < roi.height; ++i) {
    for (int j{0}; j < roi.width; ++j) {
      float val = input.at<unsigned char>(i + roi.y, j + roi.x);
      output[i * roi.width + j] = val / 256.f - 0.5f;
    }
  }
}

void CopyMatFromArray(const DataType *input, cv::Mat &output,
                      const cv::Rect &roi) {
  for (int i{0}; i < roi.height; ++i) {
    for (int j{0}; j < roi.width; ++j) {
      float val = input[i * roi.width + j];
      output.at<unsigned char>(roi.y + i, j + roi.x) = (val + 0.5f) * 256.f;
    }
  }
}

int main(int argc, char **argv) {
  const std::string kDefaultInputImage{"misc/lenna.png"};
  const std::string kDefaultOutputImage{"misc/lenna-output.png"};
  const bool kEnablePrinting = false;
  int ret = 0;
  DataType output_batch[kRowsOutput][kOutputSize];
  DataType input_batch[kRowsWindow][kWindowSize];

  cv::Mat input_img, output_img;

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

  cv::Mat input_img_pad{input_img.rows + 2, input_img.cols + 2,
                        input_img.type()};
  cv::Mat roi_input_img_pad =
      input_img_pad(cv::Rect{1, 1, input_img.cols, input_img.rows});
  input_img.copyTo(roi_input_img_pad);

  output_img = cv::Mat::zeros(input_img.size(), input_img.type());
  std::cout << "Image size: " << output_img.size() << std::endl;

  /* Send matrix */
  const int step_x = kOutputSize;
  const int step_y = kRowsOutput;

  for (int i{0}; i < input_img.rows; i += step_y) {
    for (int j{0}; j < input_img.cols; j += step_x) {
      /* Get the ROI and load the data */
      cv::Rect roi_in{j, i, kWindowSize, kRowsWindow};
      CopyMatToArray(input_img_pad, input_batch[0], roi_in);

      /* Process matrix */
      if (kEnablePrinting) {
        ama::utils::print_matrix<DataType, kRowsWindow, kWindowSize>(
            input_batch);
      }
      convolver_vector_top_accel(input_batch, kKernel, output_batch);
      if (kEnablePrinting) {
        ama::utils::print_matrix<DataType, kRowsOutput, kOutputSize>(
            output_batch);
      }

      /* Get results */
      cv::Rect roi_out{j, i, step_x, step_y};
      CopyMatFromArray(output_batch[0], output_img, roi_out);
    }
  }

  std::cout << "Finished" << std::endl;

  /* Print output matrices */
  if (argc < 3) {
    cv::imwrite(kDefaultOutputImage, output_img);
  } else {
    cv::imwrite(argv[2], output_img);
  }

  /* Co-sim patch */
  convolver_vector_top_accel(input_batch, kKernel, output_batch);

  return ret;
}
