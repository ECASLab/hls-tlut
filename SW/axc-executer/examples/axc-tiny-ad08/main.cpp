/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#ifndef EXTERNAL_BW
#define EXTERNAL_BW 20
#endif
#ifndef INTERNAL_BW
#define INTERNAL_BW 16
#endif

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

#include "examples/axc-tiny-ad08/config.hpp"
#include "executer/dense.hpp"
#include "hw-ops/MatrixMultiplyAdd.hpp"
#include "hw-ops/MatrixMultiplyAddStrassen.hpp"
#include "hw-ops/MatrixMultiplyAddWinograd.hpp"
#include "model/ad08.hpp"

/**
 * @brief Macro to select the arithmetic operators at compilation time
 *
 * This allows the accelerator simulator arithmetic choose. In practice,
 * this should not be necessary since it is inherent to what's implemented
 * on the accelerator
 */
#ifndef GET_ARITH_OPERATOR
#define GET_ARITH_OPERATOR(A, T, L, OP)                                       \
  typename std::conditional<                                                  \
      (A) == ArithApprox::LSBDROP_A,                                          \
      axc::arithmetic::approximate::lsbdrop::OP<T, T::width, T::iwidth, L>,   \
      typename std::conditional<                                              \
          (A) == ArithApprox::LSBOR_A,                                        \
          axc::arithmetic::approximate::lsbor::OP<T, T::width, T::iwidth, L>, \
          axc::arithmetic::exact::OP<T> >::type>::type
#endif

#define MATRIX_MULTIPLIER_KERNEL(Data, Kernel) \
  ama::hw::operators::Kernel<Data, kSizePE, kSizePE>;

static const std::vector<std::string> kLayerPaths = {
    "weights_dense_64_72.bin",         "biases_dense_72_.bin",
    "A_batch_normalization_72_.bin",   "B_batch_normalization_72_.bin",

    "weights_dense_1_72_72.bin",       "biases_dense_1_72_.bin",
    "A_batch_normalization_1_72_.bin", "B_batch_normalization_1_72_.bin",

    "weights_dense_2_72_8.bin",        "biases_dense_2_8_.bin",
    "A_batch_normalization_2_8_.bin",  "B_batch_normalization_2_8_.bin",

    "weights_dense_3_8_72.bin",        "biases_dense_3_72_.bin",
    "A_batch_normalization_3_72_.bin", "B_batch_normalization_3_72_.bin",

    "weights_dense_4_72_72.bin",       "biases_dense_4_72_.bin",
    "A_batch_normalization_4_72_.bin", "B_batch_normalization_4_72_.bin",

    "weights_dense_5_72_64.bin",       "biases_dense_5_64_.bin",
};

int main(int argc, char **argv) {
  static constexpr int kSizePE = 2;
  Runtime res{};

  std::shared_ptr<DataContainer> input = std::make_shared<DataContainer>();
  std::shared_ptr<DataContainer> globaloutput =
      std::make_shared<DataContainer>();

  /* Data types for the dense */
  using Q_20_10 = ap_fixed<EXTERNAL_BW, Q_INT_EXTERNAL_LAYER>;
  using Q_16_6 = ap_fixed<INTERNAL_BW, Q_INT_INTERNAL_LAYER>;

  /* Engines for the dense */
  using Engine_20_10 = MATRIX_MULTIPLIER_KERNEL(Q_20_10, MATRIX_MULTIPLIER);
  using Engine_16_6 = MATRIX_MULTIPLIER_KERNEL(Q_16_6, MATRIX_MULTIPLIER);

  /* Kernels */
  using KD_20_10 = CustomKernel::Dense<Q_20_10, Engine_20_10, kSizePE>;
  using KD_16_6 = CustomKernel::Dense<Q_16_6, Engine_16_6, kSizePE>;

  /* Accelerators */
  using Accel_20_10 = CustomAccel::Dense<Q_20_10, KD_20_10>;
  using Accel_16_6 = CustomAccel::Dense<Q_16_6, KD_16_6>;

  AccelConfig cfg_20_10 = {
      Layers::DENSE,       EXTERNAL_BW, Q_INT_EXTERNAL_LAYER, 1, 1,
      ArithApprox::EXACT_A};
  AccelConfig cfg_16_6 = {
      Layers::DENSE,       INTERNAL_BW, Q_INT_INTERNAL_LAYER, 1, 1,
      ArithApprox::EXACT_A};

  ad08accels.push_back(std::make_shared<Accel_16_6>(cfg_20_10));
  ad08accels.push_back(std::make_shared<Accel_20_10>(cfg_16_6));

  std::shared_ptr<Executer> executer = std::make_shared<Executer>(ad08accels);

  AD08 net{};

  if (argc != 5) {
    std::cerr << "Arguments does not match. It should be ./main "
                 "PATH_TO_WEIGHTS INPUT_TEST SAMPLES OUTPUT_FILE"
              << std::endl;
    return -1;
  }

  std::filesystem::path weightspath{argv[1]};
  std::filesystem::path testpath{argv[2]};
  std::string outputfile{argv[4]};

  input->num_dimensions = 3;  /* Not register the third because compatibility */
  input->dimensions[0] = 64;  /* FFT samples */
  input->dimensions[1] = 196; /* Num of Mels */
  input->dimensions[2] = std::atoi(argv[3]); /* Num samples */

  res = input->ReadFromFile(testpath);
  std::cout << "Test image read: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }

  res = net.Load(weightspath, kLayerPaths);
  std::cout << "AD08 read: " << res.msg << std::endl;
  if (res.code) {
    return -1;
  }

  /* Iterative test */
  unsigned int iterations = input->dimensions[2];
  std::cout << "Running: " << input->dimensions[2] << " samples" << std::endl;

  /* Back up the pointer for further deallocation */
  auto input_data = input->data;
  auto size = input->size;

  /* Compute offsets*/
  auto sample_offset = input->dimensions[0] * input->dimensions[1];
  auto sample_size = sample_offset * sizeof(float);

  std::cout << "Sample size: " << sample_offset << " data" << std::endl;

  /* Not register the third because compatibility */
  input->num_dimensions = 2;

  /* Prepare the global output buffer */
  globaloutput->size = size;
  globaloutput->data = new float[size / sizeof(float)];
  globaloutput->num_dimensions = input->num_dimensions;
  globaloutput->dimensions[0] = input->dimensions[0];
  globaloutput->dimensions[1] = input->dimensions[1];
  globaloutput->dimensions[2] = input->dimensions[2];

  std::cout << "Input:" << std::endl
            << "\tFirst datum: " << input->GetData()[0]
            << " Last datum: " << input->GetData()[sample_offset - 1]
            << std::endl;

  for (decltype(iterations) i = 0; i < iterations; ++i) {
    /* Jump the sample and create output container */
    input->data = new float[sample_offset];
    input->size = sample_size;
    auto data_start = &input_data[sample_offset * i];
    std::copy(data_start, data_start + sample_offset, input->data);

    std::cout << "Loading new sample: " << std::endl
              << "\tSamples: " << input->dimensions[0] << ", "
              << input->dimensions[1] << std::endl
              << "\tSize: " << input->size << " bytes" << std::endl;

    std::shared_ptr<DataContainer> output = std::make_shared<DataContainer>();

    /* Run the sample */
    res = net.Execute(executer, input, output);
    std::cout << "AD08 execute: " << res.msg << std::endl;
    if (res.code) {
      return -1;
    }

    /* Write results */
    std::copy(output->data, output->data + sample_offset,
              globaloutput->data + sample_offset * i);

    std::cout << "Output:" << std::endl
              << "\tFirst datum: " << output->GetData()[0]
              << " Last datum: " << output->GetData()[sample_offset - 1]
              << std::endl;
  }

  input->data = input_data; /* Restore for proper deallocation */
  input->size = size;

  globaloutput->WriteToFile(outputfile);
  return 0;
}
