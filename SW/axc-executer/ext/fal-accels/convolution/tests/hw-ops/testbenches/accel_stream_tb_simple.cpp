/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include <iostream>
#include <opencv2/opencv.hpp>

#include "accel_stream.hpp"
#include "accel_stream_tb.hpp"

static DataType kKernel[kKernelSize][kKernelSize] = {
    {0.0625, 0.125, 0.0625}, {0.125, 0.25, 0.125}, {0.0625, 0.125, 0.0625}};

static DataType kOnes[kRowsWindow][kWindowSize] = {
    {0.5, 0.5, 0.5, 0.5}, {0.5, 0.1, 0.1, 0.5}, {0.5, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.5}, {0.5, 0.1, 0.1, 0.5}, {0.5, 0.1, 0.1, 0.5},
    {0.5, 0.1, 0.1, 0.5}, {0.5, 0.1, 0.1, 0.5}, {0.5, 0.1, 0.1, 0.5},
    {0.5, 0.5, 0.5, 0.5}};

int main(int argc, char **argv) {
  const int kElemsPerPayload = 64 / Q_BW;

  int ret = 0;
  DataType output[kOutputSize][kOutputSize];
  PayloadType payload{0};

  /* Streaming ports */
  StreamPort stream_input{}, stream_output{};

  /* Execution params */
  DECL_EXE_PARAMS_TB;

  /* Capabilities */
  DECL_CAPS_TB;

  /* Kernels */
  DataType kernels[kKernelSize * kKernelSize];

  /* -- Write properties -- */
  WRITE_EXE_PARAM_TB(input_width, kWindowSize);    // Changed
  WRITE_EXE_PARAM_TB(input_height, kRowsWindow);   // Changed
  WRITE_EXE_PARAM_TB(output_width, kOutputSize);   // Changed
  WRITE_EXE_PARAM_TB(output_height, kRowsOutput);  // Changed
  WRITE_EXE_PARAM_TB(kernel_size, kKernelSize);    // Changed

  /* -- Execute -- */
  WRITE_EXE_PARAM_TB(execution_mode, static_cast<int>(ExecutionModes::CONFIG));
  EXECUTE_ACCEL;

  /* -- Read properties back -- */
  READ_PROPS;

  /* -- Set kernel -- */
  std::cout << "\nKernel: " << std::endl;
  for (int y = 0; y < kKernelSize; ++y) {
    for (int x = 0; x < kKernelSize; ++x) {
      int index = x + kKernelSize * y;
      kernels[index] = kKernel[y][x];
      std::cout << std::dec << " " << kernels[index];
      std::cout << std::hex << "->" << kernels[index].V;
    }
  }
  std::cout << "\n" << std::endl;

  /* -- Run with kernel -- */
  WRITE_EXE_PARAM_TB(execution_mode,
                     static_cast<int>(ExecutionModes::LOAD_KERNEL));
  EXECUTE_ACCEL;

  /* -- Upload data -- */
  int payl_elems = 0;
  std::cout << "Input: " << std::endl;
  for (int y = 0; y < kRowsWindow; ++y) {
    for (int x = 0; x < kWindowSize; ++x) {
      std::cout << std::dec << " " << kOnes[y][x];
      std::cout << std::hex << "->" << kOnes[y][x].V;

      /* If the elements are complete */
      if (kElemsPerPayload == payl_elems) {
        StreamPayload spayload{};
        spayload.data = payload;
        spayload.keep = 255;
        spayload.last = (x == (kWindowSize - 1)) && (y == (kRowsWindow - 1));
        stream_input.write(spayload);
        payl_elems = 0;
        payload = 0;
      }

      int upper = Q_BW * (payl_elems + 1) - 1;
      int lower = Q_BW * payl_elems;
      payload.range(upper, lower) = kOnes[y][x].V;
      ++payl_elems;
    }
  }
  std::cout << "\n" << std::endl;

  /* Last write */
  if (payl_elems != 0) {
    StreamPayload spayload{};
    spayload.data = payload;
    spayload.keep = -1;
    spayload.last = 1;
    stream_input.write(spayload);
    payl_elems = 0;
    std::cout << "End of stream submitted" << std::endl;
  }

  /* -- Run execute -- */
  WRITE_EXE_PARAM_TB(execution_mode, static_cast<int>(ExecutionModes::EXECUTE));
  EXECUTE_ACCEL;
  READ_PROPS;

  /* -- Retrieve data back -- */
  StreamPayload spayload;
  std::cout << "Output: " << std::endl;
  do {
    spayload = stream_output.read();
    for (int i = 0; i < kElemsPerPayload; ++i) {
      DataType datum;
      int upper = Q_BW * (i + 1) - 1;
      int lower = Q_BW * i;
      ap_uint<Q_BW> raw = spayload.data.range(upper, lower);

      datum.V = raw;
      std::cout << std::dec << " " << datum;
      std::cout << std::hex << "->" << raw;
    }
  } while (!spayload.last);
  std::cout << std::endl;

  /* -- Run execute -- */
  WRITE_EXE_PARAM_TB(execution_mode, static_cast<int>(ExecutionModes::NOP));
  EXECUTE_ACCEL;

  return ret;
}
