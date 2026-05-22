/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/**
 * @example stream-convolution.hpp
 * This is the main header of the convolution accelerator
 */

#include <convolution.hpp>

#include "wrappers/convolution.hpp"

/*
 * Dependencies:
 * Q_BW: bitwidth
 * Q_K: kernel size
 * Q_O: output size
 * Q_PES: number of PEs
 */

/* Declarations of the accelerator */
/* Define the top function name */
#define TOP_FUNCTION_NAME convolution_accel

/**
 * Declare the top function
 */
DECL_TOP_CONV_FUNCTION(TOP_FUNCTION_NAME);
/**
 * Declare the capabilities function name
 */
DECL_SET_CONV_CAPABILITIES(convolution_capabilities);
/**
 * Declare the params checking function name
 */
DECL_SET_CONV_PARAMS_CHECK(convolution_params_check);

/**
 * Version of the accelerator
 */
static constexpr int kAccelVersion = 100;

/* --- Eigen accelerator types --- */
/**
 * Maximum image size
 */
static const int kMaximumImageSize = 4096;
/**
 * Output size of the PE and accelerator width
 */
static const int kOutputSize =
    ama::hw::convolvers::Spatial<DataType, Q_K, Q_O>::outputsize;
/**
 * Input size of the PE and accelerator width
 */
static const int kWindowSize =
    ama::hw::convolvers::Spatial<DataType, Q_K, Q_O>::windowsize;
/**
 * Kernel size of the PE and accelerator width
 */
static const int kKernelSize =
    ama::hw::convolvers::Spatial<DataType, Q_K, Q_O>::kernelsize;
/**
 * Accelerator input height
 */
static const int kRowsWindow = Q_PES * kOutputSize + kKernelSize - 1;
/**
 * Accelerator height height
 */
static const int kRowsOutput = Q_PES * kOutputSize;

/* --- Implementation-specific constants --- */
/**
 * Total packets that can fit within the payload (BUSWIDTH)
 */
static constexpr int kTotalPacketsPerPayload = BUSWIDTH / Q_BW;
/**
 * Total packets that can fit within the payload
 */
static constexpr int kRowsOutputToFillPayload =
    ((BUSWIDTH / Q_BW) / kOutputSize);
/**
 * Output rows padded
 */
static constexpr int kRowsOutputPad = kRowsOutputToFillPayload > kRowsOutput
                                          ? kRowsOutputToFillPayload
                                          : kRowsOutput * 2;
/**
 * Input rows to fill the payload
 */
static constexpr int kRowsInputToFillPayload =
    ((BUSWIDTH / Q_BW) / kWindowSize);
/**
 * Input rows padded
 */
static constexpr int kRowsInputPad = kRowsInputToFillPayload > kRowsWindow
                                         ? kRowsInputToFillPayload
                                         : kRowsWindow * 2;

/* --- Implementation-specific routines --- */
/**
 * Execute function. It defines the accelerator code and wraps the vector
 * capabilities if desired.
 * @param input_img input image admitted by the PE array
 * @param kernel_img kernel admitted by the PE array
 * @param output_img output image returned by the PE array
 */
void execute(DataType input_img[kRowsWindow][kWindowSize],
             DataType kernel_img[kKernelSize][kKernelSize],
             DataType output_img[kRowsOutput][kOutputSize]);

/**
 * Retrieve data function. Defines the logic to copy from the internal buffers
 * to the stream port
 * @param stream_output output stream to get the data out to the host
 * @param output_img output image returned by the PE array
 * @param output_width width of the output
 * @param output_height height of the output
 */
void retrieve_data(StreamPort& stream_output,
                   const DataType output_img[kRowsOutputPad][kOutputSize],
                   uint output_width, uint output_height);

/**
 * Load data function. Defines the logic to copy from the input stream to the
 * internal buffers
 * @param stream_input input stream to get the data from the host
 * @param input_img input image returned by the PE array
 * @param input_width width of the input
 * @param input_height height of the input
 */
void load_data(StreamPort& stream_input,
               DataType input_img[kRowsWindow][kWindowSize], uint input_width,
               uint input_height);

/**
 * Load kernel function. Defines the logic to copy the kernel from the host
 * @param kernel_img internal kernel buffer
 * @param kernels kernel registers
 */
void load_kernel(DataType kernel_img[kKernelSize][kKernelSize],
                 DataType kernels[kKernelSize * kKernelSize]);
