/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include "wrappers/matrix.hpp"

/*
 * Dependencies:
 * Q_BW: bitwidth
 * Q_O: output size
 * Q_PES: number of PEs
 * Q_INT: integer part width
 */

/* Declarations of the accelerator */
/* Define the top function name */
#define TOP_FUNCTION_NAME activations_accel

/**
 * Declare the top function
 */
DECL_TOP_MATRIX_FUNCTION(TOP_FUNCTION_NAME);
/**
 * Declare the capabilities function name
 */
DECL_SET_MATRIX_CAPABILITIES(activations_capabilities);
/**
 * Declare the params checking function name
 */
DECL_SET_MATRIX_PARAMS_CHECK(activations_params_check);

/**
 * Version of the accelerator
 */
static constexpr int kAccelVersion = 100;

/* --- Eigen accelerator types --- */
/**
 * Output size of the PE in rows - the replication does mean multi-matrix
 * support
 */
static const int kRows = Q_PES;

/** The number of rows of a single PE is 1 */
static const int kRowsSingle = 1;

/**
 * Maximum window size supported by output
 */
static const int kMatrixMaxSize = 1024;

/**
 * Output size of the PE in cols. This should be adjusted to fit into the
 * packets to avoid wasting communication time
 */
static constexpr int kColumns =
    (Q_O * Q_BW) >= BUSWIDTH ? (Q_O * Q_BW) : BUSWIDTH / Q_BW;

/**
 * Scaling: this accelerator does not scale the output
 */
static const int kScaling = 1;

/* --- Implementation-specific constants --- */
/**
 * Total packets that can fit within the payload (BUSWIDTH)
 */
static constexpr int kTotalPacketsPerPayload = BUSWIDTH / Q_BW;
/**
 * Total packets that can fit within the payload
 */
static constexpr int kRowsToFillPayload = ((BUSWIDTH / Q_BW) / kColumns);
/**
 * Output rows padded
 */
static constexpr int kRowsPad =
    kRowsToFillPayload > kRows ? kRowsToFillPayload : kRows * 2;

/* --- Implementation-specific routines --- */
/**
 * Execute function. It defines the accelerator code and wraps the vector
 * capabilities if desired.
 * @param A first matrix input
 * @param B output matrix input
 */
void execute(DataType A[kRows][kColumns], DataType B[kRows][kColumns]);

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
