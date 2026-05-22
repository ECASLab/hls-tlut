/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <numeric>

#include <data.hpp>
#include <kernels/arithmetic.hpp>
#include <kernels/nonlinear.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact vector implementation. It uses a complete version of the
 * addition using the system addition operator by default.
 *
 * @tparam T datatype
 * @tparam NL non-linear operator
 * @tparam AO arithmetic operator
 */
template <class T, class NL = NonLinear::STL<T>>
struct ActivationOperation {
  NL nl{};
  /**
   * @brief Activation functor operation
   *
   * @param func operator function
   * @param val value to operate
   * @return T return value
   */
  T operator()(Activations func, const T val) {
    switch (func) {
      case Activations::TANH:
        return nl.tanh(val);
      case Activations::RELU:
        return nl.relu(val);
      default:
        return val;
    }
  }
};

/**
 * @brief Exact softmax implementation. It uses the C++ STL std::exp under the
 * hood. This is improper for approximations
 *
 * @tparam T datatype
 * @tparam NL non-linear operator
 * @tparam AO arithmetic operator
 */
template <class T, class NL = NonLinear::STL<T>,
          class AO = Arithmetic::Exact<T>>
struct Softmax {
  const NL nl{};
  const AO ao{};

  /** Target samples: number of samples of a batch. It must be equal to the
   * input samples */
  int target_samples = 0;
  /** Input values: number of input values */
  int input_values = 0;
  /** Output values: number of output values - it must match the input values */
  int target_outputs = 0;

  /**
   * @brief Functor operator()
   *
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T *input_data, T *output_data) {
    if (!input_data || !output_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }

    T den = 0.f;
    for (int sample = 0; sample < target_samples; ++sample) {
      for (int in = 0; in < input_values; ++in) {
        den = ao.plus(nl.exp(input_data[in + sample * input_values]), den);
      }

      /* Compute the probability */
      for (int out = 0; out < target_outputs; ++out) {
        output_data[out + sample * target_outputs] =
            nl.exp(input_data[out + sample * input_values]) / den;
      }
    }
    return Runtime{Runtime::OK, "Reduction"};  // Son muchos
  }
};
}  // namespace Exact

/**
 * @brief ReductionActivation wrapper.
 *
 * It deals with activation layer that involves all the elements to average the
 * weights and impact on the probability distribution, normalising it.
 *
 * @tparam T Datatype
 * @tparam K Execution kernel class
 * @param ilayer layer object with the execution parameters
 * @param input input data container
 * @param output output data container
 * @return Runtime
 */
template <class T, class K = Exact::Softmax<T>>
Runtime ReductionActivation(const BasicLayer *ilayer,
                            const DataContainer *input, DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<typename ::ReductionActivation>;
  using QLayerType = QLayer<T, typename ::ReductionActivation>;

  /* Handle the quantisation types in a more agnostic way */
  constexpr bool kQuantised = !std::is_same<T, float>::value;
  auto qinput = dynamic_cast<const QDataContainer<T> *>(input);
  auto qoutput = dynamic_cast<QDataContainer<T> *>(output);

  Runtime ret{};

  if (!ilayer || !input || !output) {
    ret.msg = "Cannot deal with nullptr args";
    ret.code = Runtime::INVALID_PARAMETER;
    return ret;
  }

  if (kQuantised && (!qinput || !qoutput)) {
    ret.msg = "Using quantisation and setting a non-quantised data container";
    ret.code = Runtime::INVALID_PARAMETER;
    return ret;
  }

  auto layer = dynamic_cast<const LayerType *>(ilayer);
  auto qlayer = dynamic_cast<const QLayerType *>(ilayer);

  if (!layer || (kQuantised && !qlayer)) {
    ret.msg = "Layer is not compatible with ReductionActivation";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Get layer data */
  Activations activation = layer->activation;
  if (activation != ::Activations::SOFTMAX) {
    ret.msg = "ReductionActivation only supports SoftMax";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Get input data */
  int input_dims = input->num_dimensions - 1;
  int input_samples = input->dimensions[input_dims];
  int input_values = 1;
  for (int i = 0; i < input_dims; ++i) {
    input_values *= input->dimensions[i];
  }
  const T *input_data = kQuantised
                            ? qinput->GetQData()
                            : reinterpret_cast<const T *>(input->GetData());

  /* Compute output size */
  int target_outputs = input_values;
  int target_samples = input_samples;
  int target_size = target_samples * target_outputs;
  if (output->size != (target_size * sizeof(float))) {
    output->alloc = DataAllocation::HOST;

    if (kQuantised) {
      if (qoutput->GetQData(true)) delete[] qoutput->GetQData(true);
      qoutput->SetQData(new T[target_size]);
    } else {
      if (output->GetData(true)) delete[] output->GetData(true);
      output->SetData(new float[target_size]);
    }
    output->size = target_size * sizeof(float);
  }
  output->num_dimensions = 2;
  output->dimensions[0] = target_outputs;
  output->dimensions[1] = target_samples;
  T *output_data = kQuantised ? qoutput->GetQData(true)
                              : reinterpret_cast<T *>(output->GetData(true));

  if (input->size < (input_values * input_samples * sizeof(float))) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Input size is not completely accessible"};
  }

  if (output->size < (target_outputs * target_samples * sizeof(float))) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Output size is not completely accessible"};
  }

  /* Get normalisation */
  execution_kernel.input_values = input_values;
  execution_kernel.target_samples = target_samples;
  execution_kernel.target_outputs = target_outputs;

  return execution_kernel(input_data, output_data);
}
}  // namespace Kernels
