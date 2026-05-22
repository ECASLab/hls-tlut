/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>

#include <ap_fixed.h>
#include <data.hpp>
#include <executer.hpp>
#include <layer.hpp>
#include <printmatrix.hpp>

#include "examples/axc-qlenet5/executers/axcswexecuter.hpp"
#include "examples/lenet5/model/lenet.hpp"

#include "examples/mqlenet5/config.hpp"
#include "examples/mqlenet5/model/mqlenet.hpp"
#include "examples/qlenet5/executers/fxpexecuter.hpp"
#include "examples/qlenet5/model/qlenet.hpp"

/*
 * ./builddir/examples/benchmark/benchmark-lenet5 \
 *     examples/lenet5/model-weights \
 *     examples/benchmark/model-tests/mnist-input-10000.bin \
 *     10000 \
 *     examples/benchmark/model-tests/mnist-output-10000.bin 0
 */

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

using FxPType = ap_fixed<Q_BW, Q_INT>;

void CompareSamples(const DataContainer &output, const DataContainer &expected);

void RunModel(Model *model, std::shared_ptr<Executer> exe, const int samples,
              const std::filesystem::path &inpath,
              const std::filesystem::path &wpath,
              const std::filesystem::path &refpath, const std::string &name);

int main(int argc, char **argv) {
  Runtime res{};

  DataContainer input{}, input_approx{}, expected{};
  DataContainer output_float{}, output_quantised{}, output_approx{};
  std::shared_ptr<Executer> executer_float =
      std::shared_ptr<Executer>(new Executer);
  std::shared_ptr<Executer> executer_mquantised =
      std::make_shared<Executer>(MQAccelerators);
  std::shared_ptr<Executer> executer_quantised =
      std::shared_ptr<Executer>(new FxPExecuter<FxPType>);
  std::shared_ptr<Executer> executer_approx =
      std::shared_ptr<Executer>(new AxCExecuter<FxPType, Q_DROP>);

  std::cout << "Executing with:" << std::endl
            << "\t Dense Data width: " << Q_DENSE_BW << std::endl
            << "\t Dense Data integer: " << Q_DENSE_INT << std::endl
            << "\t Conv Data width: " << Q_CONV_BW << std::endl
            << "\t Conv Data integer: " << Q_CONV_INT << std::endl
            << "\t Add1D Data width: " << Q_ADD1D_BW << std::endl
            << "\t Add1D Data integer: " << Q_ADD1D_INT << std::endl
            << "\t Add2D Data width: " << Q_ADD2D_BW << std::endl
            << "\t Add2D Data integer: " << Q_ADD2D_INT << std::endl
            << "\t Data drop: " << Q_DROP << std::endl;

  LeNet5 net_float{};
  MQLeNet5 net_mquantised{};
  QLeNet5<FxPType> net_quantised{};
  QLeNet5<FxPType> net_approx{};

  if (argc != 6) {
    std::cerr << "Arguments does not match. It should be ./main "
                 "PATH_TO_WEIGHTS INPUT_TEST SAMPLES EXPECTED_TEST RUN"
              << std::endl;
    return -1;
  }

  int samples = std::atoi(argv[3]);
  int run = std::atoi(argv[5]);
  std::filesystem::path weightspath{argv[1]};
  std::filesystem::path inputspath{argv[2]};
  std::filesystem::path expectedpath{argv[4]};

  switch (run) {
    case 0:
      RunModel(&net_float, executer_float, samples, inputspath, weightspath,
               expectedpath, "float");
      break;
    case 1:
      RunModel(&net_quantised, executer_quantised, samples, inputspath,
               weightspath, expectedpath, "quantised");
      break;
    case 2:
      RunModel(&net_approx, executer_approx, samples, inputspath, weightspath,
               expectedpath, "approx");
      break;
    case 3:
      RunModel(&net_mquantised, executer_mquantised, samples, inputspath,
               weightspath, expectedpath, "mquantised");
      break;
    default:
      std::cerr << "Invalid runner" << std::endl;
      break;
  }

  return 0;
}

void RunModel(Model *model, std::shared_ptr<Executer> exe, const int samples,
              const std::filesystem::path &inpath,
              const std::filesystem::path &wpath,
              const std::filesystem::path &refpath, const std::string &name) {
  Runtime res{};

  if (!model || !exe) {
    return;
  }

  std::shared_ptr<DataContainer> input = std::make_shared<DataContainer>();
  std::shared_ptr<DataContainer> output = std::make_shared<DataContainer>();
  std::shared_ptr<DataContainer> expected = std::make_shared<DataContainer>();

  std::cout << "Running: " << name << std::endl;

  /* --- Load input --- */
  input->num_dimensions = 4;
  input->dimensions[0] = 28; /* 28 x 28 */
  input->dimensions[1] = 28;
  input->dimensions[2] = 1;       /* 1 channel */
  input->dimensions[3] = samples; /* n-samples */

  res = input->ReadFromFile(inpath);
  std::cout << "Input sample read: " << res.msg << std::endl;
  if (res.code) return;

  /* --- Load expected outputs --- */
  expected->num_dimensions = 1;
  expected->dimensions[0] = samples; /* n-samples */

  res = expected->ReadFromFile(refpath);
  std::cout << "Output sample read: " << res.msg << std::endl;
  if (res.code) return;

  /* --- Load weights --- */
  res = model->Load(wpath, kLayerPaths);
  std::cout << "LeNet read: " << res.msg << std::endl;
  if (res.code) return;

  /* --- Execute the model --- */
  res = model->Execute(exe, input, output);
  std::cout << "LeNet - " << name << " - execute: " << res.msg << std::endl;
  if (res.code) return;

  /* --- Inference results --- */
  std::cout << "Comparing:" << name << std::endl;
  CompareSamples(*output, *expected);
}

void CompareSamples(const DataContainer &output,
                    const DataContainer &expected) {
  const int total_samples = expected.dimensions[0];
  if (total_samples != output.dimensions[1]) {
    std::cerr << "Cannot compare inference results: Samples from expected "
              << total_samples << " samples found: " << output.dimensions[1]
              << std::endl;
    return;
  }

  int correct_samples = 0;
  for (int i = 0; i < total_samples; ++i) {
    const float *optr = output.GetData() + output.dimensions[0] * i;
    const float *eptr = expected.GetData() + i;
    int output_res = std::distance(
        optr, std::max_element(optr, optr + output.dimensions[0]));
    int expected_res = static_cast<int>(*eptr);
    correct_samples += output_res == expected_res;
  }

  float correct_samples_f = static_cast<float>(correct_samples);
  float total_samples_f = static_cast<float>(total_samples);

  float accuracy = correct_samples_f / total_samples_f;

  std::cout << "--- Metrics ---" << std::endl
            << "\tAccuracy: " << accuracy << std::endl;
}
