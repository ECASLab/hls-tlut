/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>
#include <hls_stream.h>

/* --- Datatypes --- */

/* Streaming */
/**
 * AXI Bus Width
 */
#define BUSWIDTH 64
#define USERWIDTH 8
#define IDWIDTH 1
#define DESTWIDTH 1

/**
 * Alias for the unsigned integer (representation of the bus)
 */
using PayloadType = ap_uint<BUSWIDTH>;

/**
 * Struct of the payload of AXI Stream
 * The LSB of user is used for indicating the End of Window
 */
typedef struct {
  ap_uint<BUSWIDTH> data;
  ap_uint<BUSWIDTH / 8> keep;
  ap_uint<USERWIDTH> user;
  ap_uint<1> last;
} StreamPayload;

/**
 * Alias for the stream port
 */
typedef hls::stream<StreamPayload> StreamPort;

/**
 * Property port datatype
 */
using PropertyPort = ap_int<8>;

/**
 * Dimension port datatype
 */
using DimensionPort = ap_int<16>;

/**
 * Full type for generic data
 */
using FullType = ap_int<32>;
