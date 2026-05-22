/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>
#include <hls_stream.h>

#include "convolution.hpp"

/* --- Datatypes --- */

/* Streaming */
static constexpr int BusWidth = 64;
using PayloadType = ap_uint<BusWidth>;
typedef struct {
  PayloadType data;
  ap_uint<1> last;
  ap_uint<8> keep;
} StreamPayload;
typedef hls::stream<StreamPayload> StreamPort;

using DataType = ap_fixed<Q_BW, 1>;
using PropertyPort = ap_int<8>;
using DimensionPort = ap_int<16>;
using FullType = ap_int<32>;

/* --- Eigen accelerator types --- */
static const int kOutputSize =
    ama::hw::convolvers::Spatial<DataType, Q_K, Q_O>::outputsize;
static const int kWindowSize =
    ama::hw::convolvers::Spatial<DataType, Q_K, Q_O>::windowsize;
static const int kKernelSize =
    ama::hw::convolvers::Spatial<DataType, Q_K, Q_O>::kernelsize;
static const int kRowsWindow = Q_ACCEL * kOutputSize + kKernelSize - 1;
static const int kRowsOutput = Q_ACCEL * kOutputSize;

/* Concatenation indirection */
#define CAT_(lhs, rhs) lhs##_##rhs
#define STRINGIFY(a) #a

/* MACRO to declare a execution param */
#define DECL_EXE_PARAM(type, varname) \
  type &CAT_(varname, rd), type CAT_(varname, wr)
#define DECL_EXE_PARAM_TB(type, varname) \
  type CAT_(varname, rd);                \
  type CAT_(varname, wr)
#define ATTACH_EXE_PARAM_RD(varname, reg)                   \
  _Pragma(STRINGIFY(HLS INTERFACE s_axilite register port = \
                        CAT_(varname, rd) offset = (reg)))
#define ATTACH_EXE_PARAM_WR(varname, reg)                   \
  _Pragma(STRINGIFY(HLS INTERFACE s_axilite register port = \
                        CAT_(varname, wr) offset = (reg)))

/* MACRO to declare a range capability param */
#define DECL_RANGE_CAP(type, varname) \
  type &CAT_(varname, max), type &CAT_(varname, min)
#define DECL_RANGE_CAP_TB(type, varname) \
  type CAT_(varname, max);               \
  type CAT_(varname, min)
#define ATTACH_CAP_PARAM_(varname, reg)                                       \
  _Pragma(STRINGIFY(HLS INTERFACE s_axilite register port = (varname)offset = \
                        (reg)))
#define ATTACH_CAP_PARAM(varname, reg) \
  ATTACH_CAP_PARAM_(CAT_(varname, cap), reg)
#define ATTACH_CAP_PARAM_MIN(varname, reg) \
  ATTACH_CAP_PARAM_(CAT_(varname, min), reg)
#define ATTACH_CAP_PARAM_MAX(varname, reg) \
  ATTACH_CAP_PARAM_(CAT_(varname, max), reg)

/* MACRO for simple capabilities */
#define DECL_CAP(type, varname) type &CAT_(varname, cap)
#define DECL_CAP_TB(type, varname) type CAT_(varname, cap)

/* Accesors for capabilities */
#define WRITE_CAP(varname, val) CAT_(varname, cap) = (val)
#define DISABLE_CAP(varname) CAT_(varname, cap) = 0

#define WRITE_RANGE_CAP(varname, min_val, max_val) \
  CAT_(varname, min) = (min_val);                  \
  CAT_(varname, max) = (max_val)

#define ACCESS_CAP_RANGE(varname) CAT_(varname, min), CAT_(varname, max)
#define ACCESS_CAP_MIN(varname) CAT_(varname, min)
#define ACCESS_CAP_MAX(varname) CAT_(varname, max)
#define ACCESS_CAP(varname) CAT_(varname, cap)

/* Accesors for execution params */
#define READ_EXE_PARAM(varname) CAT_(varname, wr)
#define WRITE_EXE_PARAM(varname, val) CAT_(varname, rd) = (val)
#define READ_EXE_PARAM_TB(varname) CAT_(varname, rd)
#define WRITE_EXE_PARAM_TB(varname, val) CAT_(varname, wr) = (val)
#define ACCESS_EXE_PARAM(varname) CAT_(varname, rd), CAT_(varname, wr)
