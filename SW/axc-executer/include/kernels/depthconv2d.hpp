/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <numeric>

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <data.hpp>
#include <kernels/arithmetic.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact depthwise convolution implementation. It uses a complete spatial
 * convolution or the integral form.
 *
 * @tparam T Datatype
 * @tparam OP operators. Built-in by default
 */
template <class T, class OP = Arithmetic::Exact<T>>
struct DepthConv2D {
  const OP op{};

  /** Number of samples included in a batch */
  int target_samples = 0;

  /** Number of channels included in an output sample */
  int target_channels = 0;

  /** Width of an output sample */
  int target_width = 0;

  /** Height of an output sample */
  int target_height = 0;

  /** Number of channels included in an input sample */
  int input_channels = 0;

  /** Width of an input sample */
  int input_width = 0;

  /** Height of an input sample */
  int input_height = 0;

  /** Width of a kernel */
  int k_width = 0;

  /** Height of a kernel */
  int k_height = 0;

  /** Stride X */
  int stride_x = 1;

  /** Stride Y */
  int stride_y = 1;

  /** Padding X: padding for each side (i.e. 1 -> total padding is 2) */
  int padding_x = 0;

  /** Padding Y: padding for each side (i.e. 1 -> total padding is 2) */
  int padding_y = 0;

  /**
   * @brief Functor operator()
   *
   * @param k_data pointer to the kernels
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T *k_data, const T *input_data, T *output_data) {
    if (!k_data || !input_data || !output_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }

    if (input_channels != target_channels) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Depthwise preserves the number of channels. The target "
                     "channels are different from the input channels"};
    }

#ifdef WITH_OMP
#pragma omp parallel for collapse(4)
#endif
    /* Channels are treated as planes in this case */
    for (int sample = 0; sample < target_samples; ++sample) {
      for (int map = 0; map < target_channels; ++map) {
        for (int y = 0; y < target_height; ++y) {
          for (int x = 0; x < target_width; ++x) {
            T val = 0.f;
            for (int i = 0; i < k_height; ++i) {
              for (int j = 0; j < k_width; ++j) {
                /* For some reason, Keras ignores the padding when striding */
                int input_x =
                    x * stride_x + j - (stride_x != 1 ? 0 : padding_x);
                int input_y =
                    y * stride_y + i - (stride_y != 1 ? 0 : padding_y);

                int input_c = map;

                /* Get image pixel */
                /* x */
                int64_t pos = input_x;
                /* y */
                pos += input_y * input_width;
                /* plane */
                pos += input_c * input_height * input_width;
                /* sample */
                pos += sample * input_height * input_width * input_channels;

                /* Assign the val accordingly */
                T pix_val = T{0};
                if (input_x >= 0 && input_x < input_width && input_y >= 0 &&
                    input_y < input_height) {
                  pix_val = *(input_data + pos);
                }

                /* Get filter coefficient */
                pos = j + k_width * i;           /* within plane */
                pos += map * k_height * k_width; /* move plane */
                T kernel_val = *(k_data + pos);
                val = op.plus(op.mult(pix_val, kernel_val), val);
              }
            }
            int64_t pos = x + y * target_width;
            pos += map * target_height * target_width;
            pos += sample * target_height * target_width * target_channels;
            *(output_data + pos) = val;
          }
        }
      }
    }
    return Runtime{Runtime::OK, "DepthConv2D success"};
  }
};
}  // namespace Exact

/**
 * @brief DepthConv2D  2D wrapper. It performs the validation and executes the
 * DepthConv2D kernel
 *
 * @tparam T Datatype
 * @tparam K Execution kernel
 * @param ilayer Layer properties and weights
 * @param input Input buffer
 * @param output Output buffer
 * @return Runtime
 */
template <class T, class K = Exact::DepthConv2D<T>>
Runtime DepthConv2D(const BasicLayer *ilayer, const DataContainer *input,
                    DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<typename ::DepthConv2D>;
  using QLayerType = QLayer<T, typename ::DepthConv2D>;

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
    ret.msg = "Layer is not compatible with Conv2D";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

  int k_width = layer->props.dimensions[0];
  int k_height = layer->props.dimensions[1];
  int k_channels = layer->props.dimensions[2];
  T *k_data = kQuantised ? qlayer->q_data.get()
                         : reinterpret_cast<T *>(layer->data.get());

  /* Check image dimensions */
  int input_num_dims = input->num_dimensions;
  if (input_num_dims != 4) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Input must have at least 3 dimensions"};
  }

  int input_width = input->dimensions[0];
  int input_height = input->dimensions[1];
  int input_channels = input->dimensions[2];
  int input_samples = input->dimensions[3];
  const T *input_data = kQuantised
                            ? qinput->GetQData()
                            : reinterpret_cast<const T *>(input->GetData());

  if (input_channels != k_channels) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Input channels mismatches the kernel channels"};
  }

  /* Get configurations */
  int stride_x = layer->props.strides[0];
  int stride_y = layer->props.strides[1];
  int padding_x = layer->props.padding[0];
  int padding_y = layer->props.padding[1];

  /* Compute output size */
  // TODO(lleon): add support for dilatations
  int target_width =
      1 + (input->dimensions[0] + (2 * padding_x) - k_width) / stride_x;
  int target_height =
      1 + (input->dimensions[1] + (2 * padding_y) - k_height) / stride_y;
  int target_channels = k_channels;
  int target_samples = input_samples;
  int64_t output_size =
      target_width * target_height * target_channels * target_samples;

  // TODO(lleon): add support for non-CPU implementations
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
  execution_kernel.k_width = k_width;
  execution_kernel.k_height = k_height;
  execution_kernel.stride_x = stride_x;
  execution_kernel.stride_y = stride_y;
  execution_kernel.padding_x = padding_x;
  execution_kernel.padding_y = padding_y;

  /* Launch the kernel */
  return execution_kernel(k_data, input_data, output_data);
}
}  // namespace Kernels
