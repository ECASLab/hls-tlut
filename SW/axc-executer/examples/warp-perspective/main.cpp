/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include <cmath>
#include <iostream>
#include <memory>

#include <ap_fixed.h>
#include <opencv2/opencv.hpp>

#include "examples/axc-tiny-ad08/executer/dense.hpp"
#include "hw-ops/MatrixMultiplyAdd.hpp"

using QType = ap_fixed<16, 12>;

template <typename T>
void MapGenerator(QDataContainer<T> *mapx, QDataContainer<T> *mapy,
                  QDataContainer<T> *invtrans) {
  static constexpr int kSize = 2;
  static constexpr int kMatSize =
      4; /* Actually three but kSizePE does not admit it */
  using Engine = ama::hw::operators::MatrixMultiplyAdd<T, kSize, kSize>;
  using Kernel = CustomKernel::Dense<T, Engine>;

  const int width = mapx->dimensions[1];
  const int height = mapx->dimensions[2];

  /* Create kernel and configure it */
  Kernel kernel{};
  kernel.target_samples = width * height;
  kernel.target_outputs = kMatSize;
  kernel.input_values = kMatSize;

  T *invtransptr = invtrans->GetQData();

  /* Generate coordinates */
  T *invec = new T[kernel.input_values * kernel.target_samples];
  T *outvec = new T[kernel.target_outputs * kernel.target_samples];
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      invec[0 + i * width * kMatSize + j * kMatSize] = T{j};
      invec[1 + i * width * kMatSize + j * kMatSize] = T{i};
      invec[2 + i * width * kMatSize + j * kMatSize] = T{1};
      invec[3 + i * width * kMatSize + j * kMatSize] = T{1};
    }
  }

  auto ret = kernel(invtransptr, invec, outvec);

  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      mapx->GetQData()[i * width + j] =
          outvec[0 + i * width * kMatSize + j * kMatSize];
      mapy->GetQData()[i * width + j] =
          outvec[1 + i * width * kMatSize + j * kMatSize];
    }
  }

  std::cout << "Multiplication Result: " << ret.msg << std::endl;
  delete[] invec;
  delete[] outvec;
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Cannot open the file. No enough args" << std::endl;
    std::cerr << "Usage: ./warp-perspective img" << std::endl;
    return -1;
  }

  cv::Mat img = cv::imread(argv[1]);
  cv::Mat out, outhw;

  const int rows = img.rows;
  const int cols = img.cols;
  const int halfrows = rows / 2;
  const int halfcols = cols / 2;

  const float angle = -50.f * 3.1416f / 180.f;
  const float scale = 0.6f;

  /* Production */
  cv::Mat centre = cv::Mat::eye(3, 3, CV_32F);
  centre.at<float>(0, 2) = -static_cast<float>(halfcols);
  centre.at<float>(1, 2) = -static_cast<float>(halfrows);
  cv::Mat decentre = cv::Mat::eye(3, 3, CV_32F);
  decentre.at<float>(0, 2) = static_cast<float>(halfcols);
  decentre.at<float>(1, 2) = static_cast<float>(halfrows);
  cv::Mat rot = cv::Mat::eye(3, 3, CV_32F);
  rot.at<float>(0, 0) = scale * cos(angle);
  rot.at<float>(0, 1) = scale * sin(angle);
  rot.at<float>(1, 1) = rot.at<float>(0, 0);
  rot.at<float>(1, 0) = -rot.at<float>(0, 1);
  cv::Mat trans = cv::Mat::eye(3, 3, CV_32F);
  trans.at<float>(0, 2) = 100.f;
  trans.at<float>(1, 2) = 100.f;

  cv::Mat transformation = trans * decentre * rot * centre;

  /* Test the transformation */
  cv::warpPerspective(img, out, trans * decentre * rot * centre,
                      cv::Size{rows, cols}, cv::INTER_NEAREST);

  /* Use remaping manually */
  cv::Mat invtransformation = cv::Mat::zeros(4, 4, CV_32FC1);
  cv::Mat tmpinv = transformation.inv();
  tmpinv.copyTo(invtransformation(cv::Rect{0, 0, 3, 3}));

  cv::Mat mapx{img.size(), CV_32FC1};
  cv::Mat mapy{img.size(), CV_32FC1};

  /* Configure the containers */
  QDataContainer<QType> mapxdc{}, mapydc{}, invdc{};

  std::cout << "Inverse: " << invtransformation << std::endl;

  mapxdc.data = reinterpret_cast<float *>(mapx.data);
  mapydc.data = reinterpret_cast<float *>(mapy.data);
  invdc.data = reinterpret_cast<float *>(invtransformation.data);
  mapxdc.size = rows * cols * sizeof(float);
  mapxdc.num_dimensions = 3;
  mapxdc.dimensions[0] = 1;
  mapxdc.dimensions[1] = cols;
  mapxdc.dimensions[2] = rows;
  mapydc.size = rows * cols * sizeof(float);
  mapydc.num_dimensions = 3;
  mapydc.dimensions[0] = 1;
  mapydc.dimensions[1] = cols;
  mapydc.dimensions[2] = rows;
  invdc.size = 4 * 4 * sizeof(float);
  invdc.num_dimensions = 2;
  invdc.dimensions[0] = 4;
  invdc.dimensions[1] = 4;

  MapGenerator(&mapxdc, &mapydc, &invdc);
  mapxdc.Dequantise();
  mapydc.Dequantise();
  mapxdc.data = nullptr;
  mapydc.data = nullptr;
  invdc.data = nullptr;

  /* Remap */
  cv::remap(img, outhw, mapx, mapy, cv::INTER_NEAREST);

  cv::imshow("Input img", img);
  cv::imshow("Expected output img", out);
  cv::imshow("Output img", outhw);
  cv::waitKey(0);

  return 0;
}
