/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include <cmath>
#include <filesystem>
#include <iostream>
#include <memory>

#include <data.hpp>

#include <ap_fixed.h>

#include "examples/axc-tiny-ad08/executer/dense.hpp"
#include "hw-ops/MatrixMultiplyAdd.hpp"

static constexpr int kSizePE = 2;

/* Considering the same dataset as in ad08 */
using QType = ap_fixed<32, 16>;

#define CHECK(res)                                     \
  {                                                    \
    auto res_ = res;                                   \
    if (res_.code) std::cout << "Error: " << res_.msg; \
  }

using Engine = ama::hw::operators::MatrixMultiplyAdd<QType, kSizePE, kSizePE>;
using MultiplierOp = CustomKernel::Dense<QType, Engine, kSizePE>;

int main(int argc, char **argv) {
  QDataContainer<QType> input{}, output_re{}, output_imag{}, w_cos{}, w_sin{};
  MultiplierOp multiplier{};

  constexpr int kFFTSize = 512;
  constexpr int kInputSize = 1024;

  if (argc != 5) {
    std::cerr << "Cannot open the file. No enough args" << std::endl;
    std::cerr << "Usage: ./fft-1D input.bin output.bin w_cos.bin w_sin.bin"
              << std::endl;
    return -1;
  }

  std::filesystem::path input_path{argv[1]};
  std::filesystem::path re_output_path{std::string("re.") +
                                       std::string(argv[2])};
  std::filesystem::path im_output_path{std::string("im.") +
                                       std::string(argv[2])};
  std::filesystem::path w_cos_path{argv[3]};
  std::filesystem::path w_sin_path{argv[4]};

  /* Add the input data */
  input.num_dimensions = 2; /* Not register the third because compatibility */
  input.dimensions[0] = kInputSize; /* FFT samples */
  input.dimensions[1] = kSizePE;    /* Num samples */
  CHECK(input.ReadFromFile(input_path));

  /* Add the output data info */
  output_re.num_dimensions = 2;
  output_re.dimensions[0] = kFFTSize;
  output_re.dimensions[1] = kSizePE; /* Num samples */
  output_re.size = kFFTSize * kSizePE * sizeof(float);
  output_re.SetData(new float[output_re.size]);
  output_imag.num_dimensions = 2;
  output_imag.dimensions[0] = kFFTSize;
  output_imag.dimensions[1] = kSizePE; /* Num samples */
  output_imag.size = kFFTSize * kSizePE * sizeof(float);
  output_imag.SetData(new float[output_imag.size]);

  /* Add the factors */
  w_cos.num_dimensions = 2;
  w_cos.dimensions[0] = kFFTSize;
  w_cos.dimensions[1] = kInputSize;
  CHECK(w_cos.ReadFromFile(w_cos_path));
  w_sin.num_dimensions = 2;
  w_sin.dimensions[0] = kFFTSize;
  w_sin.dimensions[1] = kInputSize;
  CHECK(w_sin.ReadFromFile(w_sin_path));

  /* Configure the multiplier */
  multiplier.target_samples = kSizePE;
  multiplier.target_outputs = kFFTSize;
  multiplier.input_values = kInputSize;

  /* Run the multiplier */
  input.GetQData();
  w_cos.GetQData();
  w_sin.GetQData();

  std::cout << "Executing multipliers..." << std::endl;
  CHECK(multiplier(w_cos.GetQData(), input.GetQData(), output_re.GetQData()));
  CHECK(multiplier(w_sin.GetQData(), input.GetQData(), output_imag.GetQData()));

  std::cout << "Retrieving data..." << std::endl;
  std::cout << "Sample:\n"
            << "\tInput: " << input.GetQData(true)[100] << std::endl
            << "\tWCos: " << w_cos.GetQData(true)[1585] << std::endl
            << "\tWSin: " << w_sin.GetQData(true)[1585] << std::endl;

  std::cout << "Frequencies:\n"
            << "\tReal[0] Expected: -0.37 Obtained: " << output_re.GetQData()[0]
            << std::endl
            << "\tImag[0] Expected:  0.00 Obtained: "
            << output_imag.GetQData()[0] << std::endl
            << "\tReal[1] Expected: -0.39 Obtained: " << output_re.GetQData()[1]
            << std::endl
            << "\tImag[1] Expected: -0.03 Obtained: "
            << output_imag.GetQData()[1] << std::endl;

  output_re.WriteToFile(re_output_path);
  output_imag.WriteToFile(im_output_path);
  return 0;
}
