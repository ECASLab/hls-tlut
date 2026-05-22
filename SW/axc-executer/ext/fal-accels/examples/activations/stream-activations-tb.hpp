/*
 * Copyright 2023
 * Author: Luis G. Leon-Vega <luis.leon@ieee.org>
 */

#pragma once

#include "stream-activations.hpp"

/*
 * Testbench parameters
 */

static const int kRowsTb = Q_INPUTS_TB;
static const int kColsTb = Q_INPUTS_TB;

/**
 * Macro for executing the accel with a more comfortable line
 * <b>Example:</b>
 * @code
 * EXECUTE_ACCEL;
 * @endcode
 */
#define EXECUTE_ACCEL                                                         \
  TOP_FUNCTION_NAME(                                                          \
      stream_input, stream_output, ACCESS_EXE_PARAM(rows),                    \
      ACCESS_EXE_PARAM(columns), ACCESS_EXE_PARAM(load_matrix_mask),          \
      ACCESS_EXE_PARAM(en_accumulation), ACCESS_EXE_PARAM(en_activation),     \
      ACCESS_EXE_PARAM(scaling), ACCESS_EXE_PARAM(operation),                 \
      ACCESS_EXE_PARAM(execution_mode), ACCESS_CAP_RANGE(rows),               \
      ACCESS_CAP_RANGE(columns), ACCESS_CAP_RANGE(scaling),                   \
      ACCESS_CAP(masking), ACCESS_CAP(accumulation), ACCESS_CAP(activation),  \
      ACCESS_CAP(def_scaling), ACCESS_CAP(operations), ACCESS_CAP(num_cores), \
      ACCESS_CAP(datatype), ACCESS_CAP(num_bits_integer),                     \
      ACCESS_CAP(num_bits_fraction), ACCESS_CAP(accel_type),                  \
      ACCESS_CAP(accel_version));

/**
 * Macro for reading and printing the properties
 * <b>Example:</b>
 * @code
 * READ_PROPS;
 * @endcode
 */
#define READ_PROPS                                                            \
  std::cout << "\nExecution params"                                           \
            << "\n Rows: " << READ_EXE_PARAM_TB(rows)                         \
            << "\n Columns: " << READ_EXE_PARAM_TB(columns)                   \
            << "\n Load Matrix mask: " << std::hex                            \
            << READ_EXE_PARAM_TB(load_matrix_mask)                            \
            << "\n Enable Accumulation: " << std::dec                         \
            << READ_EXE_PARAM_TB(en_accumulation)                             \
            << "\n Enable Activation: " << READ_EXE_PARAM_TB(en_activation)   \
            << "\n Scaling: " << READ_EXE_PARAM_TB(scaling)                   \
            << "\n Operation: " << READ_EXE_PARAM_TB(operation)               \
            << "\n Execution mode: " << READ_EXE_PARAM_TB(execution_mode)     \
            << "\nCapabilities"                                               \
            << "\n Min rows: " << ACCESS_CAP_MIN(rows)                        \
            << "\n Max rows: " << ACCESS_CAP_MAX(rows)                        \
            << "\n Min columns: " << ACCESS_CAP_MIN(columns)                  \
            << "\n Max columns: " << ACCESS_CAP_MAX(columns)                  \
            << "\n Min scaling: " << ACCESS_CAP_MIN(scaling)                  \
            << "\n Max scaling: " << ACCESS_CAP_MAX(scaling)                  \
            << "\n Masking: " << ACCESS_CAP(masking)                          \
            << "\n Accumulation: " << ACCESS_CAP(accumulation)                \
            << "\n Activation: " << ACCESS_CAP(activation)                    \
            << "\n Def. Scaling: " << ACCESS_CAP(def_scaling)                 \
            << "\n Operations: " << std::hex << ACCESS_CAP(operations)        \
            << "\n Num. PEs: " << std::dec << ACCESS_CAP(num_cores)           \
            << "\n Datatype: " << std::dec << ACCESS_CAP(datatype)            \
            << "\n Num. Int. Bits: " << ACCESS_CAP(num_bits_integer)          \
            << "\n Num. Int. Frac: " << ACCESS_CAP(num_bits_fraction)         \
            << "\n Accel. Type: " << ACCESS_CAP(accel_type)                   \
            << "\n Accel. Version: " << ACCESS_CAP(accel_version) << std::dec \
            << std::endl;

/**
 * Macro for declaring the testbench variables of the ports (execution params)
 * <b>Example:</b>
 * @code
 * DECL_EXE_PARAMS_TB;
 * @endcode
 */
#define DECL_EXE_PARAMS_TB                           \
  DECL_EXE_PARAM_TB(DimensionPort, rows);            \
  DECL_EXE_PARAM_TB(DimensionPort, columns);         \
  DECL_EXE_PARAM_TB(PropertyPort, load_matrix_mask); \
  DECL_EXE_PARAM_TB(PropertyPort, en_accumulation);  \
  DECL_EXE_PARAM_TB(PropertyPort, en_activation);    \
  DECL_EXE_PARAM_TB(FullType, scaling);              \
  DECL_EXE_PARAM_TB(FullType, operation);            \
  DECL_EXE_PARAM_TB(PropertyPort, execution_mode);

/**
 * Macro for declaring the testbench variables of the ports (capabilities)
 * <b>Example:</b>
 * @code
 * DECL_CAPS_TB;
 * @endcode
 */
#define DECL_CAPS_TB                             \
  DECL_RANGE_CAP_TB(DimensionPort, rows);        \
  DECL_RANGE_CAP_TB(DimensionPort, columns);     \
  DECL_RANGE_CAP_TB(FullType, scaling);          \
  DECL_CAP_TB(PropertyPort, masking);            \
  DECL_CAP_TB(PropertyPort, accumulation);       \
  DECL_CAP_TB(PropertyPort, activation);         \
  DECL_CAP_TB(PropertyPort, def_scaling);        \
  DECL_CAP_TB(ExtendedPropertyPort, operations); \
  DECL_CAP_TB(PropertyPort, num_cores);          \
  DECL_CAP_TB(PropertyPort, datatype);           \
  DECL_CAP_TB(PropertyPort, num_bits_integer);   \
  DECL_CAP_TB(PropertyPort, num_bits_fraction);  \
  DECL_CAP_TB(PropertyPort, accel_type);         \
  DECL_CAP_TB(PropertyPort, accel_version);
