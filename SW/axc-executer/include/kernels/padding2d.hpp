/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#ifdef WITH_OMP
#include <omp.h>
#endif

#include <data.hpp>
#include <layer.hpp>
#include <runtime.hpp>

namespace Kernels {
namespace Exact {
/**
 * @brief Exact padding implementation.
 *
 * @tparam T Datatype
 * @tparam OP operators. Built-in by default
 */
template <class T, class OP = Arithmetic::Exact<T>>
struct Padding {
  const OP op{};

  /** Number of samples included in a batch */
  int target_samples = 0;

  /** Number of channels of an output sample */
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

  /** Padding top: number of pixels to add to the input */
  int padding_top = 0;

  /** Padding bottom: number of pixels to add to the input */
  int padding_bottom = 0;

  /** Padding left: number of pixels to add to the input */
  int padding_left = 0;

  /** Padding right: number of pixels to add to the input */
  int padding_right = 0;

  /**
   * @brief Functor operator()
   *
   * @param k_data pointer to the kernels
   * @param input_data pointer to the input data
   * @param output_data pointer to the output data
   * @return Runtime
   */
  Runtime operator()(const T *input_data, T *output_data) {
    if (!input_data || !output_data) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Cannot execute kernel. The operators are nullptr"};
    }

    if (input_channels != target_channels) {
      return Runtime{Runtime::INVALID_PARAMETER,
                     "Depthwise preserves the number of channels. The target "
                     "channels are different from the input channels"};
    }

    // TODO(LLEON): Only supported ZeroPadding
#ifdef WITH_OMP
#pragma omp parallel for collapse(4)
#endif
    /* Channels are treated as planes in this case */
    for (int sample = 0; sample < target_samples; ++sample) {
      for (int map = 0; map < target_channels; ++map) {
        for (int y = 0; y < target_height; ++y) {
          for (int x = 0; x < target_width; ++x) {
            /* Compute offset */
            int input_x = x - padding_left;
            int input_y = y - padding_top;

            /* Get image pixel */
            int64_t ipos = input_x;
            ipos += input_y * input_width;
            ipos += map * input_height * input_width;
            ipos += sample * input_height * input_width * input_channels;

            /* Assign the val accordingly */
            T val = T{0.f};
            if (input_x >= 0 && input_x < input_width && input_y >= 0 &&
                input_y < input_height) {
              val = *(input_data + ipos);
            }

            /* Position to the output */
            int64_t opos = x + y * target_width;
            opos += map * target_height * target_width;
            opos += sample * target_height * target_width * target_channels;
            *(output_data + opos) = val;
          }
        }
      }
    }
    return Runtime{Runtime::OK, "Padding2D success"};
  }
};
}  // namespace Exact

/**
 * @brief Padding 2D wrapper. It performs the validation and executes the
 * padding kernel
 *
 * @tparam T Datatype
 * @tparam K Execution kernel
 * @param ilayer Layer properties and weights
 * @param input Input buffer
 * @param output Output buffer
 * @return Runtime
 */
template <class T, class K = Exact::Padding<T>>
Runtime Padding2D(const BasicLayer *ilayer, const DataContainer *input,
                  DataContainer *output) {
  K execution_kernel{};
  using LayerType = Layer<typename ::Padding>;
  using QLayerType = QLayer<T, typename ::Padding>;

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
    ret.msg = "Layer is not compatible with Padding2D";
    ret.code = Runtime::INCOMPATIBLE_PARAMETER;
    return ret;
  }

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

  /* Get configurations */
  int top = layer->props.padding[0];
  int bottom = layer->props.padding[1];
  int left = layer->props.padding[2];
  int right = layer->props.padding[3];

  /* Compute output size */
  int target_width = input_width + (left + right);
  int target_height = input_height + (top + bottom);
  int target_channels = input_channels;
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
  execution_kernel.padding_top = top;
  execution_kernel.padding_bottom = bottom;
  execution_kernel.padding_left = left;
  execution_kernel.padding_right = right;

  /* Launch the kernel */
  return execution_kernel(input_data, output_data);
}
}  // namespace Kernels
