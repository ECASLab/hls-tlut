/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include "wrappers/streaming-binary.hpp"

/*
 * Dependencies:
 * Q_BW: bitwidth
 * Q_INT: integer part width
 * Q_METHOD: approximation method
 * Q_NBA: number of approximate bits in the adder
 */

/* Declarations of the accelerator */
/* Define the top function name */
#define TOP_FUNCTION_NAME addition_dataflow_accel

/**
 * Declare the top function
 */
DECL_TOP_STREAM_BINARY_FUNCTION(TOP_FUNCTION_NAME);
/**
 * Declare the capabilities function name
 */
DECL_SET_STREAM_BINARY_CAPABILITIES(addition_dataflow_capabilities);
/**
 * Declare the params checking function name
 */
DECL_SET_STREAM_BINARY_PARAMS_CHECK(addition_dataflow_params_check);

/**
 * Version of the accelerator
 */
static constexpr int kAccelVersion = 100;

/* --- Approximation constants --- */
/** Exact operators - no approximation */
#define EXACT 0
/** LSB Drop approximation */
#define LSBDROP 1
/** LSB OR approximation */
#define LSBOR 2

/* --- Eigen accelerator types --- */
/**
 * Maximum elements supported by output
 */
static const unsigned int kMaxElements = -1;

/**
 * Output size of the PE in cols. This should be adjusted to fit into the
 * packets to avoid wasting communication time
 */
static constexpr int kMinElements = BUSWIDTH / Q_BW;

/**
 * Define the matrix rows for the matrix-based wrapper
 */
static constexpr int kRows = 1;
static constexpr int kRowsPad = 1;

/**
 * Define the matrix columns for the matrix-based wrapper
 */
static constexpr int kColumns = kMinElements;

/**
 * Scaling: this accelerator does not scale the output
 */
static const int kScaling = 1;

/* --- Implementation-specific constants --- */
/**
 * Total packets that can fit within the payload (BUSWIDTH)
 */
static constexpr int kTotalPacketsPerPayload = BUSWIDTH / Q_BW;

/* --- Implementation-specific routines --- */
/**
 * Execute function. It defines the accelerator code and wraps the vector
 * capabilities if desired.
 * @param A first matrix input
 * @param B second matrix input
 * @param C output matrix input
 */
void execute(DataType A[kRows][kColumns], DataType B[kRows][kColumns],
             DataType C[kRows][kColumns]);

/**
 * Retrieve data function. Defines the logic to copy from the internal buffers
 * to the stream port
 * @param stream_output output stream to get the data out to the host
 * @param B output matrix input
 * @param rows number of rows of both matrices
 * @param columns number of columns of both matrices
 */
void retrieve_data(StreamPort& stream_output, DataType B[kRows][kColumns],
                   DimensionPort rows, DimensionPort columns);

/**
 * Load data function. Defines the logic to copy from the input stream to the
 * internal buffers
 * @param stream_input input stream to get the data from the host
 * @param A first matrix input
 * @param rows number of rows of both matrices
 * @param columns number of columns of both matrices
 */
void load_data(StreamPort& stream_input, DataType A[kRows][kColumns],
               DimensionPort rows, DimensionPort columns);
