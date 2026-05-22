/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

/**
 * @example stream-convolution-hw.cpp
 * This is the hardware implementation of the convolution accelerator
 */

#include <convolution.hpp>

#include "stream-convolution.hpp"

#define ENGINE(x) ama::hw::convolvers::x<DataType, Q_K, Q_O>;

void load_kernel(DataType kernel_img[kKernelSize][kKernelSize],
                 DataType kernels[kKernelSize * kKernelSize]) {
  /* Get kernel */
kernel_feed_y:
  for (int y = 0; y < kKernelSize; ++y) {
#pragma HLS unroll
  kernel_feed_x:
    for (int x = 0; x < kKernelSize; ++x) {
#pragma HLS unroll
      int index = x + kKernelSize * y;
      kernel_img[y][x] = kernels[index];
    }
  }
}

void load_data(StreamPort& stream_input,
               DataType input_img[kRowsWindow][kWindowSize], uint input_width,
               uint input_height) {
  static DataType input_1d_img[kRowsWindow * kWindowSize];

  if (stream_input.empty()) return;

  /* Receive input window */
  constexpr int kNumInputElems = kRowsWindow * kWindowSize;

data_stream_loop_packet:
  for (int e = 0; e < kNumInputElems; e += kTotalPacketsPerPayload) {
    StreamPayload spayload{};
    spayload = stream_input.read();
data_stream_loop_element:
    for (int p = 0; p < kTotalPacketsPerPayload; ++p) {
      const int upper = Q_BW * (p + 1) - 1;
      const int lower = Q_BW * p;

      /* Compute row and col */
      input_1d_img[e + p].V = spayload.data.range(upper, lower);
    }
    if (spayload.last) break;
  }

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_stream_copy_row:
  for (int i = 0; i < kRowsWindow; ++i) {
  data_stream_copy_col:
    for (int j = 0; j < kWindowSize; ++j) {
      input_img[i][j] = input_1d_img[i * kWindowSize + j];
    }
  }
}

void retrieve_data(StreamPort& stream_output,
                   const DataType output_img[kRowsOutputPad][kOutputSize],
                   uint output_width, uint output_height) {
  /* Transmit output window */
  constexpr int kNumOutputElems = kRowsOutput * kOutputSize;
  /* Size of the output in current run */
  const int kNumImageElems = output_height * output_width;
  /* Total number of runs */
  const int kNumExecutions = kNumImageElems / kNumOutputElems;

  static DataType output_1d_img[kRowsWindow * kWindowSize];

  /* Execution counter */
  static int execution_counter{0};
  ++execution_counter;

  /* Determine whether it is the final execution */
  bool final_execution = kNumExecutions == execution_counter;

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_retrieve_copy_row:
  for (int i = 0; i < kRowsOutput; ++i) {
  data_retrieve_copy_col:
    for (int j = 0; j < kOutputSize; ++j) {
      output_1d_img[i * kOutputSize + j] = output_img[i][j];
    }
  }

data_retrieve_packet:
  for (int e = 0; e < kNumOutputElems; e += kTotalPacketsPerPayload) {
    StreamPayload spayload{};
    bool end_of_window = kNumOutputElems <= (e + kTotalPacketsPerPayload);

  data_retrieve_elements:
    for (int p = 0; p < kTotalPacketsPerPayload; ++p) {
      const int upper = Q_BW * (p + 1) - 1;
      const int lower = Q_BW * p;

      spayload.data.range(upper, lower) = output_1d_img[e + p].V;
    }

    spayload.keep = -1;
    spayload.last = end_of_window && final_execution;
    spayload.user.range(0, 0) = end_of_window;
    stream_output.write(spayload);
  }

  /* Reset the counter if needed */
  if (final_execution) execution_counter = 0;
}

void execute(DataType input_img[kRowsWindow][kWindowSize],
             DataType kernel_img[kKernelSize][kKernelSize],
             DataType output_img[kRowsOutput][kOutputSize]) {
  using Engine = ENGINE(Q_CORE);
  ama::hw::ParallelConvolver<Q_PES, Q_PES, Engine>::Execute(
      input_img, kernel_img, output_img);
}
