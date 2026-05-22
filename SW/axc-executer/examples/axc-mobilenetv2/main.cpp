/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <memory>
#include <vector>

#include <ap_fixed.h>

#include <data.hpp>
#include <executer.hpp>
#include <layer.hpp>
#include <printmatrix.hpp>

#include "examples/axc-mobilenetv2/config.hpp"
#include "model/mobilenetv2.hpp"

/* Include the array of files to load */
#include "model/files.hpp"

static void CompareSamples(const std::shared_ptr<DataContainer> output,
                           const std::shared_ptr<DataContainer> expected);

int main(int argc, char **argv) {
  Runtime res{};

  std::shared_ptr<DataContainer> input = std::make_shared<DataContainer>();
  std::shared_ptr<DataContainer> output = std::make_shared<DataContainer>();
  std::shared_ptr<DataContainer> expected = std::make_shared<DataContainer>();
  std::shared_ptr<Executer> executer =
      std::make_shared<Executer>(mobilenetv2_accels);

  MobileNetV2 net{};

  if (argc != 5) {
    std::cerr << "Arguments does not match. It should be ./main "
                 "PATH_TO_WEIGHTS INPUT_TEST SAMPLES OUTPUT_REFERENCE"
              << std::endl;
    return -1;
  }

  std::filesystem::path weightspath{argv[1]};
  std::filesystem::path testpath{argv[2]};
  std::string expectedpath{argv[4]};

  /* --- Load input --- */
  input->num_dimensions = 4;
  input->dimensions[0] = 224;                /* Width */
  input->dimensions[1] = 224;                /* Height */
  input->dimensions[2] = 3;                  /* Channels for RGB */
  input->dimensions[3] = std::atoi(argv[3]); /* Num samples */

  res = input->ReadFromFile(testpath);
  std::cout << "Test image read: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }

  /* --- Load expected outputs --- */
  expected->num_dimensions = 1;
  expected->dimensions[0] = input->dimensions[3]; /* n-samples */

  res = expected->ReadFromFile(expectedpath);
  std::cout << "Reference read: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }

  res = net.Load(weightspath, kLayerPaths);
  std::cout << "MobileNetV2 read: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }

#ifdef DEBUG_MODE
  std::cout << "Input at 0,0 patch:" << std::endl
            << " " << input->GetData()[0 + 0 * input->dimensions[0]] << " "
            << input->GetData()[1 + 0 * input->dimensions[0]] << " "
            << input->GetData()[2 + 0 * input->dimensions[0]] << std::endl
            << " " << input->GetData()[0 + 1 * input->dimensions[0]] << " "
            << input->GetData()[1 + 1 * input->dimensions[0]] << " "
            << input->GetData()[2 + 1 * input->dimensions[0]] << std::endl
            << " " << input->GetData()[0 + 2 * input->dimensions[0]] << " "
            << input->GetData()[1 + 2 * input->dimensions[0]] << " "
            << input->GetData()[2 + 2 * input->dimensions[0]] << std::endl;
#endif

  /* Run the sample */
  res = net.Execute(executer, input, output);
  std::cout << "MobileNetV2 execute: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }

  std::cout << "MobileNetV2 output dimensions: ";
  for (int i = 0; i < output->num_dimensions; ++i) {
    std::cout << output->dimensions[i] << " ";
  }
  std::cout << std::endl;

#ifdef DEBUG_MODE
  std::cout << "Output at first patch:" << std::endl;
  for (int i = 0; i < 10; ++i) {
    std::cout << output->GetData()[i] << " ";
  }
  std::cout << std::endl;

  std::cout << "Output at last patch:" << std::endl;
  for (int i = (output->dimensions[0] - 10); i < output->dimensions[0]; ++i) {
    std::cout << output->GetData()[i] << " ";
  }
  std::cout << std::endl;
#endif

  CompareSamples(output, expected);
  return 0;
}

void CompareSamples(const std::shared_ptr<DataContainer> output,
                    const std::shared_ptr<DataContainer> expected) {
  const int total_samples = expected->dimensions[0];
  if (total_samples != output->dimensions[1]) {
    std::cerr << "Cannot compare inference results: Samples from expected "
              << total_samples << " samples found: " << output->dimensions[1]
              << std::endl;
    return;
  }

  int correct_samples = 0;
  std::cout << "Samples to evaluate: " << total_samples << std::endl;
  for (int i = 0; i < total_samples; ++i) {
    const float *optr = output->GetData() + output->dimensions[0] * i;
    const float *eptr = expected->GetData() + i;
    int output_res = std::distance(
        optr, std::max_element(optr, optr + output->dimensions[0]));
    int expected_res = static_cast<float>(*eptr);
    correct_samples += output_res == expected_res;
  }

  float correct_samples_f = static_cast<float>(correct_samples);
  float total_samples_f = static_cast<float>(total_samples);

  float accuracy = correct_samples_f / total_samples_f;

  std::cout << "--- Metrics ---" << std::endl
            << "\tAccuracy: " << accuracy << std::endl;
}
