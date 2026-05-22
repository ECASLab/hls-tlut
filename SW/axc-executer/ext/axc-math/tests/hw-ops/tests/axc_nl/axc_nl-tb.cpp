/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "axc_nl.hpp"

#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>

#include <operators.hpp>

#define XSTR(x) #x
#define STR(x) XSTR(x)

int main(int, char **) {
  static constexpr float kMinValue = Q_MIN;
  static constexpr float kMaxValue = Q_MAX;
  static constexpr int kNumSamples = Q_DATA_TB;
  static constexpr float kMaxError = Q_MAX_ERROR;
  static constexpr float kInputSpace = kMaxValue - kMinValue;

  int ret = 0;
  std::array<float, kNumSamples> sw_inputs;
  std::array<float, kNumSamples> hw_inputs;
  std::array<float, kNumSamples> sw_outputs;
  std::array<float, kNumSamples> qsw_outputs;
  std::array<float, kNumSamples> hw_outputs;
  std::array<float, kNumSamples> differences;

  /* Define the seed as fixed for reproducing the results */
  std::srand(Q_SEED);

  /* Print testbench properties */
  std::cout << "Performing approximation" << std::endl
            << "\tMethod: " << METHOD << std::endl
            << "\tCore: " << STR(Q_CORE) << std::endl
            << "\tOperation: " << STR(Q_NL_OP) << std::endl
            << "\tMinValue: " << kMinValue << std::endl
            << "\tMaxValue: " << kMaxValue << std::endl
            << "\tLUTPoints: " << Q_POINTS << std::endl
            << "\tOrder: " << Q_ORDER << std::endl
            << "\tMaxErrorAllowed: " << kMaxError << std::endl
            << "\tSamples: " << kNumSamples << std::endl;

  /* Generate samples */
  std::cout << "SwInputs:";
  for (auto &x : sw_inputs) {
    x = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) - 0.5f;
    x *= kInputSpace;
    std::cout << " " << x;
  }
  std::cout << std::endl;

  std::cout << "HwInputs:";
  for (int i = 0; i < kNumSamples; ++i) {
    hw_inputs[i] = sw_inputs[i];
    std::cout << " " << hw_inputs[i];
  }
  std::cout << std::endl;

  /* Running SW exact values */
  std::cout << "SwOutputs:";
  axc::nonlinear::exact::Exact<float> swop;
  for (int i = 0; i < kNumSamples; ++i) {
    sw_outputs[i] = swop.Q_NL_OP(sw_inputs[i]);
    std::cout << " " << sw_outputs[i];
  }
  std::cout << std::endl;

  std::cout << "QSwOutputs:";
  axc::nonlinear::exact::Exact<DataType> qswop;
  for (int i = 0; i < kNumSamples; ++i) {
    qsw_outputs[i] = qswop.Q_NL_OP(sw_inputs[i]);
    std::cout << " " << qsw_outputs[i];
  }
  std::cout << std::endl;

  /* Running HW values */
  std::cout << "HwOutputs:";
  for (int i = 0; i < kNumSamples; ++i) {
    DataType xa = DataType{hw_inputs[i]};
    DataType ya = DataType{0};
    axc_nl_accel_top(xa, ya);
    hw_outputs[i] = static_cast<float>(ya);
    std::cout << " " << hw_outputs[i];
  }
  std::cout << std::endl;

  /* Computing differences */
  std::cout << "QuantisationErrors:";
  float qmaxerror = 0.f;
  for (int i = 0; i < kNumSamples; ++i) {
    float e = abs(sw_inputs[i] - hw_inputs[i]);
    std::cout << " " << e;
    qmaxerror = std::max(qmaxerror, e);
  }
  std::cout << std::endl;

  std::cout << "SwQuantisationErrors:";
  float swmaxerror = 0.f;
  for (int i = 0; i < kNumSamples; ++i) {
    float e = abs(sw_outputs[i] - qsw_outputs[i]);
    std::cout << " " << e;
    swmaxerror = std::max(swmaxerror, e);
  }
  std::cout << std::endl;

  std::cout << "HardwareErrors:";
  float hwmaxerror = 0.f;
  for (int i = 0; i < kNumSamples; ++i) {
    float e = abs(sw_outputs[i] - hw_outputs[i]);
    std::cout << " " << e;
    hwmaxerror = std::max(hwmaxerror, e);
  }
  std::cout << std::endl;

  /* Filter by simulation */
  qmaxerror /= kInputSpace;
  float output_space = abs(swop.Q_NL_OP(kMaxValue) - swop.Q_NL_OP(kMinValue));
  swmaxerror /= output_space;
  hwmaxerror /= output_space;

  std::cout << "Maximum Errors: " << std::endl
            << "\tInputQuantisation: " << qmaxerror << std::endl
            << "\tOutputQuantisation: " << swmaxerror << std::endl
            << "\tHardware: " << hwmaxerror << std::endl;

  ret |= qmaxerror > kMaxError;
  ret |= swmaxerror > kMaxError;
  ret |= hwmaxerror > kMaxError;

  return ret;
}