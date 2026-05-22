/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#include <data.hpp>
#include <executer.hpp>
#include <layer.hpp>
#include <printmatrix.hpp>
#include "model/lenet.hpp"

static const std::vector<std::string> kLayerPaths = {
    "layer_0_conv_6-1-5-5.bin",
    "layer_1_add_6-.bin",
    "",
    "layer_2_conv_16-6-5-5.bin",
    "layer_3_add_16-.bin",
    "",
    "layer_4_dense_120-256.bin",
    "layer_5_add_120-.bin",
    "layer_6_dense_84-120.bin",
    "layer_7_add_84-.bin",
    "layer_8_dense_10-84.bin",
    "layer_9_add_10-.bin",
    ""};

int main(int argc, char **argv) {
  Runtime res{};

  std::shared_ptr<DataContainer> input = std::make_shared<DataContainer>();
  std::shared_ptr<DataContainer> output = std::make_shared<DataContainer>();
  std::shared_ptr<Executer> executer = std::make_shared<Executer>();

  std::cout << "Hello" << std::endl;
  LeNet5 net{};

  if (argc != 4) {
    std::cerr << "Arguments does not match. It should be ./main "
                 "PATH_TO_WEIGHTS INPUT_TEST SAMPLES"
              << std::endl;
    return -1;
  }

  std::filesystem::path weightspath{argv[1]};
  std::filesystem::path testpath{argv[2]};

  input->num_dimensions = 4;
  input->dimensions[0] = 28; /* 28 x 28 */
  input->dimensions[1] = 28;
  input->dimensions[2] = 1;                  /* 1 channel */
  input->dimensions[3] = std::atoi(argv[3]); /* n-samples */

  res = input->ReadFromFile(testpath);
  std::cout << "Test image read: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }
  res = net.Load(weightspath, kLayerPaths);
  std::cout << "LeNet read: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }
  res = net.Execute(executer, input, output);
  std::cout << "LeNet execute: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }

  /* Inference results */
  std::cout << "Output with " << output->num_dimensions << " dimensions"
            << std::endl;
  std::cout << "\t first dim: " << output->dimensions[0] << " elements"
            << std::endl;
  std::cout << "\t second dim: " << output->dimensions[1] << " samples"
            << std::endl;
  for (int i = 0; i < output->dimensions[1]; ++i) {
    float *optr = output->data + output->dimensions[0] * i;
    std::cout << "\t result " << i << ": "
              << std::distance(
                     optr, std::max_element(optr, optr + output->dimensions[0]))
              << std::endl;
  }
  return 0;
}
