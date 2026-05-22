/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ap_fixed.h>
#include <hls_stream.h>

#include "accel/accel.hpp"

/**
 * Standard property port for streaming-binary operations.
 */
using PropertyPort = ap_int<8>;

/**
 * Extended property port for matrix operations.
 */
using ExtendedPropertyPort = ap_int<32>;

/**
 * Standard dimension port for streaming-binary operations.
 */
using DimensionPort = ap_int<16>;

/**
 * Standard fulltype port for streaming-binary-map operations.
 */
using FullType = ap_int<32>;

/**
 * Standard datatype port for streaming-binary operations.
 */
using DataType = ap_fixed<Q_BW, Q_INT>;

#define TOP_STREAM_BINARY_FUNCTION(topname)                            \
  void topname(/* Streaming ports */                                   \
               StreamPort& stream_input_a, StreamPort& stream_input_b, \
               StreamPort& stream_output, /* Execution params */       \
               DECL_EXE_PARAM(DimensionPort, elements),                \
               DECL_EXE_PARAM(PropertyPort, load_streaming_mask),      \
               DECL_EXE_PARAM(PropertyPort, en_accumulation),          \
               DECL_EXE_PARAM(PropertyPort, en_activation),            \
               DECL_EXE_PARAM(FullType, scaling),                      \
               DECL_EXE_PARAM(FullType, operation),                    \
               DECL_EXE_PARAM(PropertyPort,                            \
                              execution_mode), /* Capabilities */      \
               DECL_RANGE_CAP(DimensionPort, elements),                \
               DECL_RANGE_CAP(FullType, scaling),                      \
               DECL_CAP(PropertyPort, masking),                        \
               DECL_CAP(PropertyPort, accumulation),                   \
               DECL_CAP(PropertyPort, activation),                     \
               DECL_CAP(PropertyPort, def_scaling),                    \
               DECL_CAP(ExtendedPropertyPort, operations),             \
               DECL_CAP(PropertyPort, num_cores),                      \
               DECL_CAP(PropertyPort, datatype),                       \
               DECL_CAP(PropertyPort, num_bits_integer),               \
               DECL_CAP(PropertyPort, num_bits_fraction),              \
               DECL_CAP(PropertyPort, accel_type),                     \
               DECL_CAP(PropertyPort, accel_version))

/**
 * Declares the top function with a personalised name. They are likely to
 * be in header files
 * <b>Example</b>
 * \code{.c}
 * @example
 * // Header file:
 * DECL_TOP_STREAM_BINARY_FUNCTION(myaccel);
 * \endcode
 * @param topname name of the top function
 */
#define DECL_TOP_STREAM_BINARY_FUNCTION(topname) \
  TOP_STREAM_BINARY_FUNCTION(topname)

/**
 * Defines the start of the definition of the top function with a personalised.
 * name. They are likely to be in a source code file.
 * <b>Example</b>
 * \code{.c}
 * // --- Header file ---
 * DECL_TOP_STREAM_BINARY_FUNCTION(myaccel);
 *
 * // --- Source code file (core file) ---
 * DECL_SET_STREAM_BINARY_CAPABILITIES (myaccel_caps);
 * DECL_SET_STREAM_BINARY_PARAMS_CHECK (myaccel_checker);
 *
 * // Definitions of the auxiliar functions
 *
 * BEGIN_DEF_TOP_STREAM_BINARY_FUNCTION (myaccel, myaccel_caps, myaccel_checker)
 * {
 *   // Definition of the top function
 * }
 * END_DEF_TOP_STREAM_BINARY_FUNCTION ()
 * \endcode
 * @param topname name of the top function
 * @param caps name of the function to set the accelerator's capabilities
 * @param exe_params name of the function to check the accelerator's parameters
 */
#define BEGIN_DEF_TOP_STREAM_BINARY_FUNCTION(topname, caps, exe_params)       \
  TOP_STREAM_BINARY_FUNCTION(topname) {                                       \
    /* -- Set capabilities -- */                                              \
    caps(ACCESS_CAP_RANGE(elements), ACCESS_CAP_RANGE(scaling),               \
         ACCESS_CAP(masking), ACCESS_CAP(accumulation),                       \
         ACCESS_CAP(activation), ACCESS_CAP(def_scaling),                     \
         ACCESS_CAP(operations), ACCESS_CAP(num_cores), ACCESS_CAP(datatype), \
         ACCESS_CAP(num_bits_integer), ACCESS_CAP(num_bits_fraction),         \
         ACCESS_CAP(accel_type),                                              \
         ACCESS_CAP(accel_version)); /* -- Check params -- */                 \
    exe_params(ACCESS_EXE_PARAM(elements),                                    \
               ACCESS_EXE_PARAM(load_streaming_mask),                         \
               ACCESS_EXE_PARAM(en_accumulation),                             \
               ACCESS_EXE_PARAM(en_activation), ACCESS_EXE_PARAM(scaling),    \
               ACCESS_EXE_PARAM(operation), ACCESS_EXE_PARAM(execution_mode));

/**
 * Closes the definition of a top function
 * <b>Example</b>
 * \code{.c}
 * // --- Header file ---
 * DECL_TOP_STREAM_BINARY_FUNCTION(myaccel);
 *
 * // --- Source code file (core file) ---
 * DECL_SET_STREAM_BINARY_CAPABILITIES (myaccel_caps);
 * DECL_SET_STREAM_BINARY_PARAMS_CHECK (myaccel_checker);
 *
 * // Definitions of the auxiliar functions
 *
 * BEGIN_DEF_TOP_STREAM_BINARY_FUNCTION (myaccel, myaccel_caps, myaccel_checker)
 * {
 *   // Definition of the top function
 * }
 * END_DEF_TOP_STREAM_BINARY_FUNCTION ()
 * \endcode
 */
#define END_DEF_TOP_STREAM_BINARY_FUNCTION() }

#define SET_STREAM_BINARY_CAPABILITIES(funcname)                               \
  inline static void funcname(                                                 \
      DECL_RANGE_CAP(DimensionPort, elements),                                 \
      DECL_RANGE_CAP(FullType, scaling), DECL_CAP(PropertyPort, masking),      \
      DECL_CAP(PropertyPort, accumulation),                                    \
      DECL_CAP(PropertyPort, activation), DECL_CAP(PropertyPort, def_scaling), \
      DECL_CAP(ExtendedPropertyPort, operations),                              \
      DECL_CAP(PropertyPort, num_cores), DECL_CAP(PropertyPort, datatype),     \
      DECL_CAP(PropertyPort, num_bits_integer),                                \
      DECL_CAP(PropertyPort, num_bits_fraction),                               \
      DECL_CAP(PropertyPort, accel_type),                                      \
      DECL_CAP(PropertyPort, accel_version))

/**
 * Declares the function of setting the accelerator's capabilities with a
 * personalised name. They are likely to be in header files.
 * <b>Example</b>
 * \code{.c}
 * // Header file:
 * DECL_SET_STREAM_BINARY_CAPABILITIES(myaccel_caps);
 * \endcode
 * @param funcname name of the function
 */
#define DECL_SET_STREAM_BINARY_CAPABILITIES(funcname) \
  SET_STREAM_BINARY_CAPABILITIES(topname)

/**
 * Defines the capabilities function with a personalised name.
 * They are likely to be in a source code file. Capabilities function is
 * usually combined with CAPs-related macros, such as WRITE_RANGE_CAP,
 * DISABLE_CAP, WRITE_CAP
 * <b>Example</b>
 * \code{.c}
 * // --- Source code file (core file) ---
 * DECL_SET_STREAM_BINARY_CAPABILITIES(myaccel_caps);
 *
 * DEF_SET_STREAM_BINARY_CAPABILITIES (myaccel_caps) {
 *   WRITE_RANGE_CAP(input_width, kWindowSize, kWindowSize);
 *   DISABLE_CAP(stride);
 *   WRITE_CAP(num_inputs, 1);
 * }
 * \endcode
 * @param funcname name of the function
 */
#define DEF_SET_STREAM_BINARY_CAPABILITIES(funcname) \
  SET_STREAM_BINARY_CAPABILITIES(funcname)

#define SET_STREAM_BINARY_PARAMS_CHECK(funcname)                              \
  inline static void funcname(                                                \
      DECL_EXE_PARAM(DimensionPort, elements),                                \
      DECL_EXE_PARAM(PropertyPort, load_streaming_mask),                      \
      DECL_EXE_PARAM(PropertyPort, en_accumulation),                          \
      DECL_EXE_PARAM(PropertyPort, en_activation),                            \
      DECL_EXE_PARAM(FullType, scaling), DECL_EXE_PARAM(FullType, operation), \
      DECL_EXE_PARAM(PropertyPort, execution_mode))

/**
 * Declares the function of checking accelerator's parameters with a
 * personalised name. They are likely to be in header files.
 * <b>Example</b>
 * \code{.c}
 * // Header file:
 * DECL_SET_STREAM_BINARY_PARAMS_CHECK(myaccel_caps);
 * \endcode
 * @param funcname name of the function
 */
#define DECL_SET_STREAM_BINARY_PARAMS_CHECK(funcname) \
  SET_STREAM_BINARY_PARAMS_CHECK(funcname)

/**
 * Defines the parameters checking function with a personalised name.
 * They are likely to be in a source code file. Capabilities function is
 * usually combined with CAPs-related macros, such as:
 * CHECK_EQUALITY_AND_FALLBACK, CHECK_RANGE_AND_FALLBACK
 * <b>Example</b>
 * \code{.c}
 * // --- Source code file (core file) ---
 * DECL_SET_STREAM_BINARY_PARAMS_CHECK (myaccel_checker);
 *
 * DEF_SET_STREAM_BINARY_CAPABILITIES (myaccel_checker) {
 *   CHECK_EQUALITY_AND_FALLBACK(output_width, kOutputSize, kOutputSize);
 *   CHECK_RANGE_AND_FALLBACK(input_height, kWindowSize, kRowsWindow,
 * kWindowSize);
 * }
 * \endcode
 * @param funcname name of the function
 */
#define DEF_SET_STREAM_BINARY_PARAMS_CHECK(funcname) \
  SET_STREAM_BINARY_PARAMS_CHECK(funcname)

/**
 * Shortcut to access the streaming port A
 */
#define STREAM_BINARY_INPUT_A stream_input_a

/**
 * Shortcut to access the streaming port B
 */
#define STREAM_BINARY_INPUT_B stream_input_b

/**
 * Shortcut to access the output port
 */
#define STREAM_BINARY_OUTPUT stream_output
