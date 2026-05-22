/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>
#include <hls_stream.h>

#include "accel/accel.hpp"

/**
 * Standard property port for convolutions.
 */
using PropertyPort = ap_int<8>;

/**
 * Standard dimension port for convolutions.
 */
using DimensionPort = ap_int<16>;

/**
 * Standard fulltype port for convolutions.
 */
using FullType = ap_int<32>;

/**
 * Standard datatype port for convolutions.
 */
using DataType = ap_fixed<Q_BW, Q_INT>;

#define TOP_CONV_FUNCTION(topname)                                             \
  void                                                                         \
  topname(/* Streaming port */                                                 \
          StreamPort& stream_input,                                            \
          StreamPort& stream_output, /* Execution params */                    \
          DECL_EXE_PARAM(DimensionPort, input_width),                          \
          DECL_EXE_PARAM(DimensionPort, input_height),                         \
          DECL_EXE_PARAM(DimensionPort, output_width),                         \
          DECL_EXE_PARAM(DimensionPort, output_height),                        \
          DECL_EXE_PARAM(PropertyPort, kernel_size),                           \
          DECL_EXE_PARAM(PropertyPort, num_kernels),                           \
          DECL_EXE_PARAM(FullType, output_scaling),                            \
          DECL_EXE_PARAM(PropertyPort, padding_type),                          \
          DECL_EXE_PARAM(PropertyPort, stride_x),                              \
          DECL_EXE_PARAM(PropertyPort, stride_y),                              \
          DECL_EXE_PARAM(PropertyPort, dilatation_x),                          \
          DECL_EXE_PARAM(PropertyPort, dilatation_y),                          \
          DECL_EXE_PARAM(PropertyPort, execution_mode), /* Capabilities */     \
          DECL_RANGE_CAP(DimensionPort, input_width),                          \
          DECL_RANGE_CAP(DimensionPort, input_height),                         \
          DECL_RANGE_CAP(DimensionPort, output_width),                         \
          DECL_RANGE_CAP(DimensionPort, output_height),                        \
          DECL_CAP(PropertyPort, stride), DECL_CAP(PropertyPort, dilatation),  \
          DECL_CAP(PropertyPort, padding), DECL_RANGE_CAP(FullType, scaling),  \
          DECL_CAP(PropertyPort, bias), DECL_CAP(PropertyPort, num_inputs),    \
          DECL_CAP(PropertyPort, num_kernels),                                 \
          DECL_CAP(PropertyPort, kernel_size),                                 \
          DECL_CAP(PropertyPort, num_cores), DECL_CAP(PropertyPort, datatype), \
          DECL_CAP(PropertyPort, num_bits_integer),                            \
          DECL_CAP(PropertyPort, num_bits_fraction), /* Kernels */             \
          DECL_CAP(PropertyPort, accel_type),                                  \
          DECL_CAP(PropertyPort, accel_version), DataType kernels[Q_K * Q_K])

/**
 * Declares the top function with a personalised name. They are likely to
 * be in header files
 * <b>Example</b>
 * \code{.c}
 * // Header file:
 * DECL_TOP_CONV_FUNCTION(myaccel);
 * \endcode
 * @param topname name of the top function
 */
#define DECL_TOP_CONV_FUNCTION(topname) TOP_CONV_FUNCTION(topname)

/**
 * Defines the start of the definition of the top function with a personalised.
 * name. They are likely to be in a source code file.
 * <b>Example</b>
 * \code{.c}
 * // --- Header file ---
 * DECL_TOP_CONV_FUNCTION(myaccel);
 *
 * // --- Source code file (core file) ---
 * DECL_SET_CONV_CAPABILITIES (myaccel_caps);
 * DECL_SET_CONV_PARAMS_CHECK (myaccel_checker);
 *
 * // Definitions of the auxiliar functions
 *
 * BEGIN_DEF_TOP_CONV_FUNCTION (myaccel, myaccel_caps, myaccel_checker) {
 *   // Definition of the top function
 * }
 * END_DEF_TOP_CONV_FUNCTION ()
 * \endcode
 * @param topname name of the top function
 * @param caps name of the function to set the accelerator's capabilities
 * @param exe_params name of the function to check the accelerator's parameters
 */
#define BEGIN_DEF_TOP_CONV_FUNCTION(topname, caps, exe_params)                \
  TOP_CONV_FUNCTION(topname) {                                                \
    /* -- Set capabilities -- */                                              \
    caps(ACCESS_CAP_RANGE(input_width), ACCESS_CAP_RANGE(input_height),       \
         ACCESS_CAP_RANGE(output_width), ACCESS_CAP_RANGE(output_height),     \
         ACCESS_CAP(stride), ACCESS_CAP(dilatation), ACCESS_CAP(padding),     \
         ACCESS_CAP_RANGE(scaling), ACCESS_CAP(bias), ACCESS_CAP(num_inputs), \
         ACCESS_CAP(num_kernels), ACCESS_CAP(kernel_size),                    \
         ACCESS_CAP(num_cores), ACCESS_CAP(datatype),                         \
         ACCESS_CAP(num_bits_integer), ACCESS_CAP(num_bits_fraction),         \
         ACCESS_CAP(accel_type),                                              \
         ACCESS_CAP(accel_version)); /* -- Check params -- */                 \
    exe_params(                                                               \
        ACCESS_EXE_PARAM(input_width), ACCESS_EXE_PARAM(input_height),        \
        ACCESS_EXE_PARAM(output_width), ACCESS_EXE_PARAM(output_height),      \
        ACCESS_EXE_PARAM(kernel_size), ACCESS_EXE_PARAM(num_kernels),         \
        ACCESS_EXE_PARAM(output_scaling), ACCESS_EXE_PARAM(padding_type),     \
        ACCESS_EXE_PARAM(stride_x), ACCESS_EXE_PARAM(stride_y),               \
        ACCESS_EXE_PARAM(dilatation_x), ACCESS_EXE_PARAM(dilatation_y),       \
        ACCESS_EXE_PARAM(execution_mode));

/**
 * Closes the definition of a top function
 * <b>Example</b>
 * \code{.c}
 * // --- Header file ---
 * DECL_TOP_CONV_FUNCTION(myaccel);
 *
 * // --- Source code file (core file) ---
 * DECL_SET_CONV_CAPABILITIES (myaccel_caps);
 * DECL_SET_CONV_PARAMS_CHECK (myaccel_checker);
 *
 * // Definitions of the auxiliar functions
 *
 * BEGIN_DEF_TOP_CONV_FUNCTION (myaccel, myaccel_caps, myaccel_checker) {
 *   // Definition of the top function
 * }
 * END_DEF_TOP_CONV_FUNCTION ()
 * \endcode
 */
#define END_DEF_TOP_CONV_FUNCTION() }

#define SET_CONV_CAPABILITIES(funcname)                                       \
  inline static void funcname(                                                \
      DECL_RANGE_CAP(DimensionPort, input_width),                             \
      DECL_RANGE_CAP(DimensionPort, input_height),                            \
      DECL_RANGE_CAP(DimensionPort, output_width),                            \
      DECL_RANGE_CAP(DimensionPort, output_height),                           \
      DECL_CAP(PropertyPort, stride), DECL_CAP(PropertyPort, dilatation),     \
      DECL_CAP(PropertyPort, padding), DECL_RANGE_CAP(FullType, scaling),     \
      DECL_CAP(PropertyPort, bias), DECL_CAP(PropertyPort, num_inputs),       \
      DECL_CAP(PropertyPort, num_kernels),                                    \
      DECL_CAP(PropertyPort, kernel_size), DECL_CAP(PropertyPort, num_cores), \
      DECL_CAP(PropertyPort, datatype),                                       \
      DECL_CAP(PropertyPort, num_bits_integer),                               \
      DECL_CAP(PropertyPort, num_bits_fraction),                              \
      DECL_CAP(PropertyPort, accel_type),                                     \
      DECL_CAP(PropertyPort, accel_version))

/**
 * Declares the function of setting the accelerator's capabilities with a
 * personalised name. They are likely to be in header files.
 * <b>Example</b>
 * \code{.c}
 * // Header file:
 * DECL_SET_CONV_CAPABILITIES(myaccel_caps);
 * \endcode
 * @param funcname name of the function
 */
#define DECL_SET_CONV_CAPABILITIES(funcname) SET_CONV_CAPABILITIES(topname)

/**
 * Defines the capabilities function with a personalised name.
 * They are likely to be in a source code file. Capabilities function is
 * usually combined with CAPs-related macros, such as WRITE_RANGE_CAP,
 * DISABLE_CAP, WRITE_CAP
 * <b>Example</b>
 * \code{.c}
 * // --- Source code file (core file) ---
 * DECL_SET_CONV_CAPABILITIES(myaccel_caps);
 *
 * DEF_SET_CONV_CAPABILITIES (myaccel_caps) {
 *   WRITE_RANGE_CAP(input_width, kWindowSize, kWindowSize);
 *   DISABLE_CAP(stride);
 *   WRITE_CAP(num_inputs, 1);
 * }
 * \endcode
 * @param funcname name of the function
 */
#define DEF_SET_CONV_CAPABILITIES(funcname) SET_CONV_CAPABILITIES(funcname)

#define SET_CONV_PARAMS_CHECK(funcname)                                     \
  inline static void funcname(DECL_EXE_PARAM(DimensionPort, input_width),   \
                              DECL_EXE_PARAM(DimensionPort, input_height),  \
                              DECL_EXE_PARAM(DimensionPort, output_width),  \
                              DECL_EXE_PARAM(DimensionPort, output_height), \
                              DECL_EXE_PARAM(PropertyPort, kernel_size),    \
                              DECL_EXE_PARAM(PropertyPort, num_kernels),    \
                              DECL_EXE_PARAM(FullType, output_scaling),     \
                              DECL_EXE_PARAM(PropertyPort, padding_type),   \
                              DECL_EXE_PARAM(PropertyPort, stride_x),       \
                              DECL_EXE_PARAM(PropertyPort, stride_y),       \
                              DECL_EXE_PARAM(PropertyPort, dilatation_x),   \
                              DECL_EXE_PARAM(PropertyPort, dilatation_y),   \
                              DECL_EXE_PARAM(PropertyPort, execution_mode))

/**
 * Declares the function of checking accelerator's parameters with a
 * personalised name. They are likely to be in header files.
 * <b>Example</b>
 * \code{.c}
 * // Header file:
 * DECL_SET_CONV_PARAMS_CHECK(myaccel_caps);
 * \endcode
 * @param funcname name of the function
 */
#define DECL_SET_CONV_PARAMS_CHECK(funcname) SET_CONV_PARAMS_CHECK(funcname)

/**
 * Defines the parameters checking function with a personalised name.
 * They are likely to be in a source code file. Capabilities function is
 * usually combined with CAPs-related macros, such as:
 * CHECK_EQUALITY_AND_FALLBACK, CHECK_RANGE_AND_FALLBACK
 * <b>Example</b>
 * \code{.c}
 * // --- Source code file (core file) ---
 * DECL_SET_CONV_PARAMS_CHECK (myaccel_checker);
 *
 * DEF_SET_CONV_CAPABILITIES (myaccel_checker) {
 *   CHECK_EQUALITY_AND_FALLBACK(output_width, kOutputSize, kOutputSize);
 *   CHECK_RANGE_AND_FALLBACK(input_height, kWindowSize, kRowsWindow,
 * kWindowSize);
 * }
 * \endcode
 * @param funcname name of the function
 */
#define DEF_SET_CONV_PARAMS_CHECK(funcname) SET_CONV_PARAMS_CHECK(funcname)
