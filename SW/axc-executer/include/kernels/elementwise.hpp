/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <numeric>

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <data.hpp>
#include <kernels/activations.hpp>
#include <kernels/arithmetic.hpp>
#include <kernels/nonlinear.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact vector add implementation. It uses a complete version of the
 * addition using the system addition operator.
 *
 * @tparam T datatype
 * @tparam NL non-linear operator
 * @tparam AO arithmetic operator
 */
template <class T, class NL = NonLinear::STL<T>,
          class AO = axc::arithmetic::exact::Add<T>>
struct ElementWise {
  AO ao{};
  ActivationOperation<T, NL> act{};

  /** Number of samples included in a batch */
  int target_samples = 0;

  /** Number of inputs/outputs of the vectors to add */
  int target_outputs = 0;

  /** Activation used at the end of the layer */
  Activations activation;

  /**
   * @brief Functor operator()
   *
   * @param add_data pointer to the biases
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T *add_data, const T *input_data, T *output_data) {
    if (!input_data || !output_data || !add_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }
#ifdef WITH_OMP
#pragma omp parallel for
#endif
    for (int sample = 0; sample < target_samples; ++sample) {
      for (int elem = 0; elem < target_outputs; ++elem) {
        int pos = elem + sample * target_outputs;
        T val = ao.operator()(add_data[elem], input_data[pos]);
        output_data[pos] = act(activation, val);
      }
    }

    return Runtime{Runtime::OK, "Add1D success"};
  }
};

/**
 * @brief Exact matrix add implementation. It uses a complete version of the
 * addition using the system addition operator.
 *
 * @tparam T datatype
 * @tparam NL non-linear operator
 * @tparam AO arithmetic operator
 */
template <class T, class NL = NonLinear::STL<T>,
          class AO = axc::arithmetic::exact::Add<T>>
struct ElementWise2D {
  AO ao{};
  ActivationOperation<T, NL> act{};

  /** Number of samples included in a batch */
  int target_samples = 0;

  /** Number of channels included in an output sample */
  int target_channels = 0;

  /** Width of a sample. It must match the input */
  int target_width = 0;

  /** Height of a sample. It must match the input */
  int target_height = 0;

  /** Number of channels included in an input sample */
  int input_channels = 0;

  /** Width of a sample. It must match the output */
  int input_width = 0;

  /** Height of a sample. It must match the output */
  int input_height = 0;

  /** Activation used at the end of the layer */
  Activations activation;

  /**
   * @brief Functor operator()
   *
   * @param add_data pointer to the biases
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T *add_data, const T *input_data, T *output_data) {
    if (!input_data || !output_data || !add_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }
#ifdef WITH_OMP
#pragma omp parallel for collapse(2)
#endif
    for (int sample = 0; sample < target_samples; ++sample) {
      for (int map = 0; map < target_channels; ++map) {
        T add_val = add_data[map];
        for (int y = 0; y < target_height; ++y) {
          for (int x = 0; x < target_width; ++x) {
            int input_x = x;
            int input_y = y;
            int input_c = map;

            /* Get image pixel */
            int64_t pos = input_x;                       /* x */
            pos += input_y * input_width;                /* y */
            pos += input_c * input_height * input_width; /* plane */
            pos += input_height * input_width * input_channels *
                   sample; /* sample */
            T val = ao(*(input_data + pos), add_val);

            /* Save the result */
            *(output_data + pos) = act(activation, val);
          }
        }
      }
    }

    return Runtime{Runtime::OK, "Add2D success"};
  }
};
}  // namespace Exact

/**
 * @brief Vector addition - 1D. It performs the vector addition with plugable
 * kernel
 *
 * @tparam T Datatype
 * @tparam K Execution kernel class
 * @tparam P Layer properties
 * @param ilayer Layer properties
 * @param input Input buffer
 * @param output Output buffer
 * @return Runtime
 */
template <class T, class K = Exact::ElementWise<T>, class P = ::Add>
Runtime ElementWise(const BasicLayer *ilayer, const DataContainer *input,
                    DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<P>;
  using QLayerType = QLayer<T, P>;

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
    ret.msg = "Layer is not compatible with Add";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Get layer data */
  if (layer->props.num_dimensions != 1) {
    ret.msg = "Add layer cannot deal with more than 1 dimension";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  int add_elements = layer->props.dimensions[0];
  Activations activation = layer->activation;
  T *add_data = kQuantised ? qlayer->q_data.get()
                           : reinterpret_cast<T *>(layer->data.get());

  /* Get input data */
  int input_dims = input->num_dimensions - 1;
  if (input_dims != 1) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER, "Input is not 1D"};
  }

  int input_values = 1;
  int input_samples = input->dimensions[input_dims];
  for (int i = 0; i < input_dims; ++i) {
    input_values *= input->dimensions[i];
  }
  const T *input_data = kQuantised
                            ? qinput->GetQData()
                            : reinterpret_cast<const T *>(input->GetData());

  if (input_values != add_elements) {
    ret.msg =
        "Add Layer cannot deal with operands with different number of elements";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Compute output size */
  int target_outputs = input_values;
  int target_samples = input_samples;
  int64_t target_size = target_outputs * target_samples;

  // FIXME: Cannot compute the size in this way
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

  /* Prepare the kernel */
  execution_kernel.target_samples = target_samples;
  execution_kernel.target_outputs = target_outputs;
  execution_kernel.activation = activation;

  /* Launch the kernel */
  return execution_kernel(add_data, input_data, output_data);
}

/**
 * @brief Matrix addition - 2D. It performs the vector addition with plugable
 * kernel
 *
 * @tparam T Datatype
 * @tparam K Execution kernel class
 * @tparam P Layer properties
 * @param ilayer Layer properties
 * @param input Input buffer
 * @param output Output buffer
 * @return Runtime
 */
template <class T, class K = Exact::ElementWise2D<T>, class P = ::Add>
Runtime ElementWise2D(const BasicLayer *ilayer, const DataContainer *input,
                      DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<P>;
  using QLayerType = QLayer<T, P>;

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
    ret.msg = "Layer is not compatible with Add";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  /* Get layer data */
  int add_channels = layer->props.dimensions[0];
  Activations activation = layer->activation;
  T *add_data = kQuantised ? qlayer->q_data.get()
                           : reinterpret_cast<T *>(layer->data.get());

  /* Get input data */
  if (input->num_dimensions != 4) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Input must be 2D with 4 dims"};
  }
  int input_width = input->dimensions[0];
  int input_height = input->dimensions[1];
  int input_channels = input->dimensions[2];
  int input_samples = input->dimensions[3];
  int input_size = input->size;
  const T *input_data = kQuantised
                            ? qinput->GetQData()
                            : reinterpret_cast<const T *>(input->GetData());

  if (input_channels != add_channels) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Input channels mismatches the add channels"};
  }

  /* Compute output size */
  int target_width = input_width;
  int target_height = input_height;
  int target_channels = input_channels;
  int target_samples = input_samples;
  int64_t output_size = input_size >> 2;

  if (output->size != (output_size * sizeof(float))) {
    output->alloc = DataAllocation::HOST;

    if (kQuantised) {
      if (qoutput->GetQData(true)) delete[] qoutput->GetQData(true);
      qoutput->SetQData(new T[output_size]);
    } else {
      if (output->GetData(true)) delete[] output->GetData(true);
      output->SetData(new float[output_size]);
    }
    output->size = output_size * sizeof(float);
  }
  output->num_dimensions = 4;
  output->dimensions[0] = target_width;
  output->dimensions[1] = target_height;
  output->dimensions[2] = target_channels;
  output->dimensions[3] = target_samples;
  T *output_data = kQuantised ? qoutput->GetQData(true)
                              : reinterpret_cast<T *>(output->GetData(true));

  if (input->size < input_channels * input_height * input_width *
                        input_samples * sizeof(float)) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Input size is not completely accessible"};
  }

  if (output->size < target_channels * target_width * target_height *
                         target_samples * sizeof(float)) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Output size is not completely accessible"};
  }

  /* Prepare the kernel */
  execution_kernel.target_samples = target_samples;
  execution_kernel.target_channels = target_channels;
  execution_kernel.target_width = target_width;
  execution_kernel.target_height = target_height;
  execution_kernel.input_channels = input_channels;
  execution_kernel.input_width = input_width;
  execution_kernel.input_height = input_height;
  execution_kernel.activation = activation;

  /* Launch the kernel */
  return execution_kernel(add_data, input_data, output_data);
}

}  // namespace Kernels
