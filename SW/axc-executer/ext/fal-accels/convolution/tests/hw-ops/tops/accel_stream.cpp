/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "accel.hpp"

static constexpr int kTotalPacketsPerPayload = 64 / Q_BW;
static constexpr int kRowsOutputToFillPayload = ((64 / Q_BW) / kOutputSize);
static constexpr int kRowsOutputPad = kRowsOutputToFillPayload > kRowsOutput
                                          ? kRowsOutputToFillPayload
                                          : kRowsOutput * 2;
static constexpr int kRowsInputToFillPayload = ((64 / Q_BW) / kWindowSize);
static constexpr int kRowsInputPad = kRowsInputToFillPayload > kRowsWindow
                                         ? kRowsInputToFillPayload
                                         : kRowsWindow * 2;

#define ENGINE(x) ama::hw::convolvers::x<DataType, Q_K, Q_O>;

inline static void accel_set_capabilities(
    DECL_RANGE_CAP(DimensionPort, input_width),
    DECL_RANGE_CAP(DimensionPort, input_height),
    DECL_RANGE_CAP(DimensionPort, output_width),
    DECL_RANGE_CAP(DimensionPort, output_height),
    DECL_CAP(PropertyPort, stride), DECL_CAP(PropertyPort, dilatation),
    DECL_CAP(PropertyPort, padding), DECL_RANGE_CAP(FullType, scaling),
    DECL_CAP(PropertyPort, bias), DECL_CAP(PropertyPort, num_inputs),
    DECL_CAP(PropertyPort, num_kernels), DECL_CAP(PropertyPort, kernel_size),
    DECL_CAP(PropertyPort, num_cores), DECL_CAP(PropertyPort, datatype),
    DECL_CAP(PropertyPort, num_bits_integer),
    DECL_CAP(PropertyPort, num_bits_fraction)) {
  WRITE_RANGE_CAP(input_width, kWindowSize, kWindowSize);
  WRITE_RANGE_CAP(input_height, kRowsWindow, kRowsWindow);
  WRITE_RANGE_CAP(output_width, kOutputSize, kOutputSize);
  WRITE_RANGE_CAP(output_height, kRowsOutput, kRowsOutput);

  DISABLE_CAP(stride);
  DISABLE_CAP(dilatation);
  DISABLE_CAP(padding);
  DISABLE_CAP(bias);

  WRITE_CAP(num_inputs, 1);
  WRITE_RANGE_CAP(scaling, 1, 1);
  WRITE_CAP(num_kernels, 1);
  WRITE_CAP(kernel_size, kKernelSize);
  WRITE_CAP(num_cores, Q_ACCEL);
  WRITE_CAP(datatype, static_cast<int>(Datatypes::FXP_NORM));
  WRITE_CAP(num_bits_integer, 1);
  WRITE_CAP(num_bits_fraction, Q_BW - 1);
}

inline static void accel_check_params(
    DECL_EXE_PARAM(DimensionPort, input_width),
    DECL_EXE_PARAM(DimensionPort, input_height),
    DECL_EXE_PARAM(DimensionPort, output_width),
    DECL_EXE_PARAM(DimensionPort, output_height),
    DECL_EXE_PARAM(PropertyPort, kernel_size),
    DECL_EXE_PARAM(PropertyPort, num_kernels),
    DECL_EXE_PARAM(FullType, output_scaling),
    DECL_EXE_PARAM(PropertyPort, padding_type),
    DECL_EXE_PARAM(PropertyPort, stride_x),
    DECL_EXE_PARAM(PropertyPort, stride_y),
    DECL_EXE_PARAM(PropertyPort, dilatation_x),
    DECL_EXE_PARAM(PropertyPort, dilatation_y),
    DECL_EXE_PARAM(PropertyPort, execution_mode)) {
  /* Dimensions */
  if (kWindowSize != READ_EXE_PARAM(input_width))
    WRITE_EXE_PARAM(input_width, kWindowSize);
  else
    WRITE_EXE_PARAM(input_width, READ_EXE_PARAM(input_width));
  if (kOutputSize != READ_EXE_PARAM(output_width))
    WRITE_EXE_PARAM(output_width, kOutputSize);
  else
    WRITE_EXE_PARAM(output_width, READ_EXE_PARAM(output_width));
  if (kWindowSize > READ_EXE_PARAM(input_height) ||
      READ_EXE_PARAM(input_height) > kRowsWindow)
    WRITE_EXE_PARAM(input_height, kWindowSize);
  else
    WRITE_EXE_PARAM(input_height, READ_EXE_PARAM(input_height));
  if (kOutputSize > READ_EXE_PARAM(output_height) ||
      READ_EXE_PARAM(output_height) > kRowsOutput)
    WRITE_EXE_PARAM(output_height, kOutputSize);
  else
    WRITE_EXE_PARAM(output_height, READ_EXE_PARAM(output_height));

  /* Runtime params */
  WRITE_EXE_PARAM(kernel_size, READ_EXE_PARAM(kernel_size) == PropertyPort{Q_K}
                                   ? READ_EXE_PARAM(kernel_size)
                                   : PropertyPort{Q_K});
  WRITE_EXE_PARAM(num_kernels, READ_EXE_PARAM(num_kernels) == PropertyPort{1}
                                   ? READ_EXE_PARAM(num_kernels)
                                   : PropertyPort{1});
  WRITE_EXE_PARAM(output_scaling, READ_EXE_PARAM(output_scaling) == FullType{1}
                                      ? READ_EXE_PARAM(output_scaling)
                                      : FullType{1});

  const PropertyPort kDefPadding = static_cast<int>(PaddingType::NONE);
  WRITE_EXE_PARAM(padding_type, READ_EXE_PARAM(padding_type) == kDefPadding
                                    ? READ_EXE_PARAM(padding_type)
                                    : kDefPadding);
  WRITE_EXE_PARAM(stride_x, READ_EXE_PARAM(stride_x) == PropertyPort{0}
                                ? READ_EXE_PARAM(stride_x)
                                : PropertyPort{0});
  WRITE_EXE_PARAM(stride_y, READ_EXE_PARAM(stride_y) == PropertyPort{0}
                                ? READ_EXE_PARAM(stride_y)
                                : PropertyPort{0});
  WRITE_EXE_PARAM(dilatation_x, READ_EXE_PARAM(dilatation_x) == PropertyPort{0}
                                    ? READ_EXE_PARAM(dilatation_x)
                                    : PropertyPort{0});
  WRITE_EXE_PARAM(dilatation_y, READ_EXE_PARAM(dilatation_y) == PropertyPort{0}
                                    ? READ_EXE_PARAM(dilatation_y)
                                    : PropertyPort{0});
  WRITE_EXE_PARAM(execution_mode, READ_EXE_PARAM(execution_mode));
}

inline static void load_kernel(DataType kernel_img[kKernelSize][kKernelSize],
                               DataType kernels[kKernelSize * kKernelSize]) {
#pragma HLS inline
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

inline static void load_data(StreamPort& stream_input,
                             DataType input_img[kRowsWindow][kWindowSize],
                             uint input_width, uint input_height) {
#pragma HLS inline
  DataType input_1d_img[kRowsWindow * kWindowSize];
#pragma HLS ARRAY_PARTITION variable = input_1d_img complete dim = 0

  if (stream_input.empty()) return;

  /* Receive input window */
  constexpr int kNumInputElems = kRowsWindow * kWindowSize;

data_stream_loop:
  for (int e = 0; e < kNumInputElems; e += kTotalPacketsPerPayload) {
#pragma HLS pipeline
    StreamPayload spayload{};
    spayload = stream_input.read();
    for (int p = 0; p < kTotalPacketsPerPayload; ++p) {
#pragma HLS unroll
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
#pragma HLS unroll
  data_stream_copy_col:
    for (int j = 0; j < kWindowSize; ++j) {
#pragma HLS unroll
      input_img[i][j] = input_1d_img[i * kWindowSize + j];
    }
  }
}

inline static void retrieve_data(
    StreamPort& stream_output,
    const DataType output_img[kRowsOutputPad][kOutputSize], uint output_width,
    uint output_height) {
#pragma HLS inline
  /* Transmit output window */
  constexpr int kNumOutputElems = kRowsOutput * kOutputSize;
  DataType output_1d_img[kRowsWindow * kWindowSize];
#pragma HLS ARRAY_PARTITION variable = output_1d_img complete dim = 0

  /* This copy is required to guarantee a proper transformation.
     Techniques like pointers did not work */
data_retrieve_copy_row:
  for (int i = 0; i < kRowsOutput; ++i) {
#pragma HLS unroll
  data_retrieve_copy_col:
    for (int j = 0; j < kOutputSize; ++j) {
#pragma HLS unroll
      output_1d_img[i * kOutputSize + j] = output_img[i][j];
    }
  }

data_retrieve:
  for (int e = 0; e < kNumOutputElems; e += kTotalPacketsPerPayload) {
#pragma HLS pipeline
    StreamPayload spayload{};
    for (int p = 0; p < kTotalPacketsPerPayload; ++p) {
#pragma HLS unroll
      const int upper = Q_BW * (p + 1) - 1;
      const int lower = Q_BW * p;

      spayload.data.range(upper, lower) = output_1d_img[e + p].V;
    }
    spayload.keep = -1;
    spayload.last = kNumOutputElems <= (e + kTotalPacketsPerPayload);
    stream_output.write(spayload);
  }
}

inline static void execute(DataType input_img[kRowsWindow][kWindowSize],
                           DataType kernel_img[kKernelSize][kKernelSize],
                           DataType output_img[kRowsOutput][kOutputSize]) {
#pragma HLS inline
  using Engine = ENGINE(Q_CONV_CORE);
  ama::hw::ParallelConvolver<Q_ACCEL, Q_ACCEL, Engine>::Execute(
      input_img, kernel_img, output_img);
}

/* The convolution happens in row-major but it is transferred by columns */
void accel_stream_top_accel(
    /* Streaming port */
    StreamPort& stream_input, StreamPort& stream_output,
    /* Execution params */
    DECL_EXE_PARAM(DimensionPort, input_width),
    DECL_EXE_PARAM(DimensionPort, input_height),
    DECL_EXE_PARAM(DimensionPort, output_width),
    DECL_EXE_PARAM(DimensionPort, output_height),
    DECL_EXE_PARAM(PropertyPort, kernel_size),
    DECL_EXE_PARAM(PropertyPort, num_kernels),
    DECL_EXE_PARAM(FullType, output_scaling),
    DECL_EXE_PARAM(PropertyPort, padding_type),
    DECL_EXE_PARAM(PropertyPort, stride_x),
    DECL_EXE_PARAM(PropertyPort, stride_y),
    DECL_EXE_PARAM(PropertyPort, dilatation_x),
    DECL_EXE_PARAM(PropertyPort, dilatation_y),
    DECL_EXE_PARAM(PropertyPort, execution_mode),
    /* Capabilities */
    DECL_RANGE_CAP(DimensionPort, input_width),
    DECL_RANGE_CAP(DimensionPort, input_height),
    DECL_RANGE_CAP(DimensionPort, output_width),
    DECL_RANGE_CAP(DimensionPort, output_height),
    DECL_CAP(PropertyPort, stride), DECL_CAP(PropertyPort, dilatation),
    DECL_CAP(PropertyPort, padding), DECL_RANGE_CAP(FullType, scaling),
    DECL_CAP(PropertyPort, bias), DECL_CAP(PropertyPort, num_inputs),
    DECL_CAP(PropertyPort, num_kernels), DECL_CAP(PropertyPort, kernel_size),
    DECL_CAP(PropertyPort, num_cores), DECL_CAP(PropertyPort, datatype),
    DECL_CAP(PropertyPort, num_bits_integer),
    DECL_CAP(PropertyPort, num_bits_fraction),
    /* Kernels */
    DataType kernels[kKernelSize * kKernelSize]) {
  /* -- Define the binding - execution -- */
  /* See directive file */

  uint execution_mode = READ_EXE_PARAM(execution_mode);

  /* -- Set capabilities -- */
  accel_set_capabilities(
      ACCESS_CAP_RANGE(input_width), ACCESS_CAP_RANGE(input_height),
      ACCESS_CAP_RANGE(output_width), ACCESS_CAP_RANGE(output_height),
      ACCESS_CAP(stride), ACCESS_CAP(dilatation), ACCESS_CAP(padding),
      ACCESS_CAP_RANGE(scaling), ACCESS_CAP(bias), ACCESS_CAP(num_inputs),
      ACCESS_CAP(num_kernels), ACCESS_CAP(kernel_size), ACCESS_CAP(num_cores),
      ACCESS_CAP(datatype), ACCESS_CAP(num_bits_integer),
      ACCESS_CAP(num_bits_fraction));
  /* -- Check params -- */
  accel_check_params(
      ACCESS_EXE_PARAM(input_width), ACCESS_EXE_PARAM(input_height),
      ACCESS_EXE_PARAM(output_width), ACCESS_EXE_PARAM(output_height),
      ACCESS_EXE_PARAM(kernel_size), ACCESS_EXE_PARAM(num_kernels),
      ACCESS_EXE_PARAM(output_scaling), ACCESS_EXE_PARAM(padding_type),
      ACCESS_EXE_PARAM(stride_x), ACCESS_EXE_PARAM(stride_y),
      ACCESS_EXE_PARAM(dilatation_x), ACCESS_EXE_PARAM(dilatation_y),
      ACCESS_EXE_PARAM(execution_mode));

  execution_mode = READ_EXE_PARAM(execution_mode);

  /* Temporal buffers */
  DataType input_img[kRowsWindow][kWindowSize];
  DataType output_img[kRowsOutputPad][kOutputSize];
  DataType kernel_img[kKernelSize][kKernelSize];
#pragma HLS ARRAY_PARTITION variable = input_img complete dim = 0
#pragma HLS ARRAY_PARTITION variable = kernel_img complete dim = 0
#pragma HLS ARRAY_PARTITION variable = output_img complete dim = 0

  uint input_width = READ_EXE_PARAM(input_width);
  uint input_height = READ_EXE_PARAM(input_height);
  uint output_width = READ_EXE_PARAM(output_width);
  uint output_height = READ_EXE_PARAM(output_height);

  if (stream_input.empty()) return;

  /* ----------------------------------------------------------------------- */
  /* --                           Load kernel                             -- */
  /* ----------------------------------------------------------------------- */
  load_kernel(kernel_img, kernels);

  /* ----------------------------------------------------------------------- */
  /* --                         Load image block                          -- */
  /* ----------------------------------------------------------------------- */
  load_data(stream_input, input_img, input_width, input_height);

  /* ----------------------------------------------------------------------- */
  /* --                           Execute accel                            --*/
  /* ----------------------------------------------------------------------- */
  execute(input_img, kernel_img, output_img);

  /* ----------------------------------------------------------------------- */
  /* --                           Retrieve data                            --*/
  /* ----------------------------------------------------------------------- */
  retrieve_data(stream_output, output_img, output_width, output_height);
}
