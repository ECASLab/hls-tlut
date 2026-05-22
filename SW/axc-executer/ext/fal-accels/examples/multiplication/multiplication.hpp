/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>
#include <ap_int.h>
#include <hls_stream.h>

static constexpr int kBusWidth = 64;
static constexpr int kPropertyWidth = 16;
static constexpr int kKeepWidth = kBusWidth / 8;
static constexpr int kDataWidth = Q_BW;
static constexpr int kIntWidth = Q_INT;

using DataType = ap_fixed<kDataWidth, kIntWidth>;
using PayloadType = ap_uint<kBusWidth>;
using PropertyType = ap_uint<kPropertyWidth>;

static constexpr unsigned int kDataPerPayload = kBusWidth / kDataWidth;

typedef struct {
  PayloadType data;
  ap_uint<kKeepWidth> keep;
  ap_uint<1> last;
} StreamType;

using StreamPort = hls::stream<StreamType>;

// TODO: You can change this later
static constexpr int kSizePE = 2;
static constexpr int kRows =
    kSizePE;  // TODO: Make it change according to Q_PES
static constexpr int kColumns = (kBusWidth / kDataWidth) & (~0x1);

static constexpr int kMinColumns = kColumns;
static constexpr int kMinRows = kRows;

void Execute(const DataType A[kRows][kColumns],
             const DataType B[kColumns][kColumns], DataType C[kRows][kColumns]);

void Load(StreamPort &stream_input, DataType A[kRows][kColumns],
          DataType B[kColumns][kColumns]);

void WriteBack(StreamPort &stream_output, DataType C[kRows][kColumns]);

void multiplication_accel_top(
    /* Stream ports */
    StreamPort &input, StreamPort &output,

    /* Runtime params */
    PropertyType &rows, PropertyType &columns,

    /* Capabilities */
    PropertyType &min_rows, PropertyType &min_columns, PropertyType &max_rows,
    PropertyType &max_columns);