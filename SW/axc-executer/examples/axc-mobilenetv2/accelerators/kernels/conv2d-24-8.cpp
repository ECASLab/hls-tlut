/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include "conv2d-24-8.hpp"  // NOLINT

#include <algorithm>
#include <iterator>

#include <printmatrix.hpp>
#include <runtime.hpp>

/* Define the environment for the accelerator. It must be placed before
   its inclusion */

#define Q_BW DataWidthBits
#define Q_INT IntWidthBits
#define Q_K 3
#define Q_O 2
#define Q_PES 4
#define Q_CORE Spatial

/* Once the environment is set, include the convolution including the sources */
// clang-format off
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wint-in-bool-context"
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunused-label"
#include <fal-accels/examples/convolution/stream-convolution.hpp>
#include <fal-accels/examples/convolution/stream-convolution-tb.hpp>
#include <fal-accels/examples/convolution/stream-convolution-hw.cpp>  // NOLINT
#include <fal-accels/examples/convolution/stream-convolution-core.cpp>  // NOLINT
#include "mat-stream.hpp" // NOLINT
#pragma GCC diagnostic pop
// clang-format on

using namespace Kernels::FAL;  // NOLINT

Runtime Conv2D_Spatial_24_8::operator()(
    const Conv2D_Spatial_24_8::Q *k_data,
    const Conv2D_Spatial_24_8::Q *input_data,
    Conv2D_Spatial_24_8::Q *output_data) {
  /*
   * Note: please, notice that the logic is pretty similar to the testbench.
   *       You can have a look at stream-convolution-tb.cpp to compare.
   *       There are macros that are specific for the accelerator. So, the
   *       recommendation is to stick to your own accelerator's testbench.
   */
  if (!k_data || !input_data || !output_data) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Cannot execute kernel. The operators are nullptr"};
  }
  int execution_counter = 0;

  /* Execution params */
  DECL_EXE_PARAMS_TB;

  /* Capabilities */
  DECL_CAPS_TB;

  /* -- Write properties -- */
  WRITE_EXE_PARAM_TB(input_width, this->input_width);   /* Unused */
  WRITE_EXE_PARAM_TB(input_height, this->input_height); /* Unused */
  WRITE_EXE_PARAM_TB(output_width, this->target_width);
  WRITE_EXE_PARAM_TB(output_height, this->target_height);
  WRITE_EXE_PARAM_TB(kernel_size, this->k_width);

  const int step_x = kOutputSize;
  const int step_y = kRowsOutput;

  std::cout << "Custom engine executing on:\n"
            << "\tInput: " << this->input_width << "x" << this->input_height
            << std::endl
            << "\tOutput: " << this->target_width << "x" << this->target_height
            << std::endl
            << "\tPadding: " << this->padding_x << "x" << this->padding_y
            << std::endl
            << "\tStride: " << this->stride_x << "x" << this->stride_y
            << std::endl
            << "\tKernel: " << this->k_width << "x" << this->k_height
            << std::endl;

  const int opitch = this->target_width;
  const int ipitch = this->input_width;

  std::size_t out_size = this->target_width * this->target_height *
                         this->target_channels * this->target_samples;
  std::size_t kernel_size = kKernelSize * kKernelSize;

  /* Initialise arrays */
  std::fill(output_data, output_data + out_size, Conv2D_Spatial_24_8::Q{0});

  /* Adequate the padding accordingly: this is required when the maximum kernel
     size is the only managed by the accelerator. The idea is to centre the
     input window */
  const int padding_x_ = this->padding_x + ((kKernelSize - this->k_width) >> 1);
  const int padding_y_ =
      this->padding_y + ((kKernelSize - this->k_height) >> 1);

  /* -- Cycle the Accelerator -- */
  /* Important: Cannot parallelise since the accelerator has static variables
                and they can be misinterpreted */
  for (int sample = 0; sample < this->target_samples; ++sample) {
    for (int map = 0; map < this->target_channels; ++map) {
      for (int c = 0; c < this->input_channels; ++c) {
        Conv2D_Spatial_24_8::Q kernels[kKernelSize * kKernelSize];
        std::fill(kernels, kernels + kernel_size, Conv2D_Spatial_24_8::Q{0});

        /* Compute offsets */
        const int outoffset = map * this->target_width * this->target_height +
                              sample * this->target_width *
                                  this->target_height * this->target_channels;
        const int inoffset = c * this->input_width * this->input_height +
                             sample * this->input_width * this->input_height *
                                 this->input_channels;
        const int koffset =
            c * this->k_width * this->k_height +
            map * this->k_width * this->k_height * this->input_channels;

        /* Load kernel: centre it accordingly to the maximum size */
        auto kptr = k_data + koffset;
        LoadKernel(kptr, kernels, kKernelSize, kKernelSize, this->k_width,
                   this->k_height);

        /* Prepare pointers */
        auto iptr = input_data + inoffset;
        auto optr = output_data + outoffset;

        /* Batch buffers */
        DataType output_batch[kRowsOutput][kOutputSize];
        DataType input_batch[kRowsWindow][kWindowSize];

        for (int y{0}; y < this->target_height; y += step_y) {
          for (int x{0}; x < this->target_width; x += step_x) {
            StreamPort stream_input{}, stream_output{};

            /* For some reason, Keras ignores the padding when striding */
            int input_x = x - padding_x_;
            int input_y = y - padding_y_;

            /* Get image block and upload */
            CopyMatToArray(iptr, input_batch[0], input_x, input_y, kWindowSize,
                           kRowsWindow, ipitch, this->input_height,
                           kWindowSize);
            UploadImage(input_batch, stream_input);

            /* Execute */
            WRITE_EXE_PARAM_TB(execution_mode,
                               static_cast<int>(ExecutionModes::EXECUTE));
            EXECUTE_ACCEL;
            ++execution_counter;

            /* Download result and store */
            DownloadImage(stream_output, output_batch);
            CopyMatFromArray(output_batch[0], optr, x, y, step_x, step_y,
                             kOutputSize, this->target_width,
                             this->target_height);
          }
        }
      }
    }
  }

  std::cout << "\tExecution took: " << execution_counter << " runs"
            << std::endl;
  return Runtime{Runtime::OK, "Conv2D success"};
}
