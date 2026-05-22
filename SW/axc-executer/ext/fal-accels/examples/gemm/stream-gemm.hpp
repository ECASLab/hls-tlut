/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/**
 * @example stream-gemm.hpp
 * This is the main header of the GEMM accelerator
 */

#include <hw-ops/MatrixMultiplyAdd.hpp>

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
#define TOP_FUNCTION_NAME gemm_accel

/**
 * Declare the top function
 */
DECL_TOP_MATRIX_FUNCTION(TOP_FUNCTION_NAME);
/**
 * Declare the capabilities function name
 */
DECL_SET_MATRIX_CAPABILITIES(gemm_capabilities);
/**
 * Declare the params checking function name
 */
DECL_SET_MATRIX_PARAMS_CHECK(gemm_params_check);

/**
 * Version of the accelerator
 */
static constexpr int kAccelVersion = 100;

/* --- Eigen accelerator types --- */
/**
 * Output size of the PE in rows - the replication does mean multi-matrix
 * support
 */
static const int kRows = Q_PES * Q_O;
static const int kRowsSingle = Q_O;

/**
 * Maximum window size supported by GEMM
 */
static const int kMatrixMaxSize = 1024;

/**
 * Output size of the PE in cols
 */
static const int kColumns = Q_O;

/**
 * Scaling
 */
static const int kScaling = 2 * Q_O;

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
 * @param B second matrix input
 * @param C cumulative and output matrix
 */
void execute(DataType A[kRows][kColumns], DataType B[kRows][kColumns],
             DataType C[kRows][kColumns]);

/**
 * Retrieve data function. Defines the logic to copy from the internal buffers
 * to the stream port
 * @param stream_output output stream to get the data out to the host
 * @param C third matrix input
 * @param en_accumulation enable the accumulation or output the result
 * @param rows number of rows of both matrices
 * @param columns number of columns of both matrices
 */
void retrieve_data(StreamPort& stream_output, DataType C[kRows][kColumns],
                   PropertyPort en_accumulation, DimensionPort rows,
                   DimensionPort columns);

/**
 * Load data function. Defines the logic to copy from the input stream to the
 * internal buffers
 * @param stream_input input stream to get the data from the host
 * @param A first matrix input
 * @param B second matrix input
 * @param C third matrix input
 * @param load_matrix_mask mask which matrices are loaded
 * @param rows number of rows of both matrices
 * @param columns number of columns of both matrices
 */
void load_data(StreamPort& stream_input, DataType A[kRows][kColumns],
               DataType B[kRows][kColumns], DataType C[kRows][kColumns],
               PropertyPort load_matrix_mask, DimensionPort rows,
               DimensionPort columns);
