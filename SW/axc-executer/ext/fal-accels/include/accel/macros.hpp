/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/**
 * Concatenate two strings
 */
#define CAT_(lhs, rhs) lhs##_##rhs

/**
 * Convert a name to string
 */
#define STRINGIFY(a) #a

/**
 * Convert to string
 */
#define TOSTRING(x) STRINGIFY(x)

/**
 * Declare an execution parameter. It is adequate for function declaration.
 * It declares two variables: one ending in _rd and another in _wr to define
 * read and write ports.
 * @param type type of the variable
 * @param varname variable name to declare (prefix)
 */
#define DECL_EXE_PARAM(type, varname) \
  type &CAT_(varname, rd), type CAT_(varname, wr)

/**
 * Declare an execution parameter. It is adequate for in-body declarations.
 * It declares two variables: one ending in _rd and another in _wr to define
 * read and write ports.
 * @param type type of the variable
 * @param varname variable name to declare (prefix)
 */
#define DECL_EXE_PARAM_TB(type, varname) \
  type CAT_(varname, rd);                \
  type CAT_(varname, wr)

/**
 * Attaches a read execution parameter to a register. Prefer to set this kind
 * of declarations in a directive file
 * @param varname name of the variable
 * @param reg register in hex. i.e. 0x420
 */
#define ATTACH_EXE_PARAM_RD(varname, reg)                   \
  _Pragma(STRINGIFY(HLS INTERFACE s_axilite register port = \
                        CAT_(varname, rd) offset = (reg)))

/**
 * Attaches a write execution parameter to a register. Prefer to set this kind
 * of declarations in a directive file
 * @param varname name of the variable
 * @param reg register in hex. i.e. 0x420
 */
#define ATTACH_EXE_PARAM_WR(varname, reg)                   \
  _Pragma(STRINGIFY(HLS INTERFACE s_axilite register port = \
                        CAT_(varname, wr) offset = (reg)))

/**
 * Declare a range capability. It is adequate for function declaration.
 * It declares two variables: one ending in _min and another in _max to define
 * the limits of the capability.
 * @param type type of the variable
 * @param varname variable name to declare (prefix)
 */
#define DECL_RANGE_CAP(type, varname) \
  type &CAT_(varname, max), type &CAT_(varname, min)

/**
 * Declare a range capability. It is adequate for in-body declaration.
 * It declares two variables: one ending in _min and another in _max to define
 * the limits of the capability.
 * @param type type of the variable
 * @param varname variable name to declare (prefix)
 */
#define DECL_RANGE_CAP_TB(type, varname) \
  type CAT_(varname, max);               \
  type CAT_(varname, min)

#define ATTACH_CAP_PARAM_(varname, reg)                                       \
  _Pragma(STRINGIFY(HLS INTERFACE s_axilite register port = (varname)offset = \
                        (reg)))

/**
 * Attaches a capability parameter to a register. Prefer to set this kind
 * of declarations in a directive file
 * @param varname name of the variable
 * @param reg register in hex. i.e. 0x420
 */
#define ATTACH_CAP_PARAM(varname, reg) \
  ATTACH_CAP_PARAM_(CAT_(varname, cap), reg)

/**
 * Attaches a minimum capability parameter to a register. Prefer to set this
 * kind of declarations in a directive file
 * @param varname name of the variable
 * @param reg register in hex. i.e. 0x420
 */
#define ATTACH_CAP_PARAM_MIN(varname, reg) \
  ATTACH_CAP_PARAM_(CAT_(varname, min), reg)

/**
 * Attaches a maximum capability parameter to a register. Prefer to set this
 * kind of declarations in a directive file
 * @param varname name of the variable
 * @param reg register in hex. i.e. 0x420
 */
#define ATTACH_CAP_PARAM_MAX(varname, reg) \
  ATTACH_CAP_PARAM_(CAT_(varname, max), reg)

/**
 * Declare a simple capability. It is adequate for function declaration.
 * It declares only a variable and attaches _cap as a suffix
 * @param type type of the variable
 * @param varname variable name to declare (prefix)
 */
#define DECL_CAP(type, varname) type &CAT_(varname, cap)

/**
 * Declare a simple capability. It is adequate for in-body declaration.
 * It declares only a variable and attaches _cap as a suffix
 * @param type type of the variable
 * @param varname variable name to declare (prefix)
 */
#define DECL_CAP_TB(type, varname) type CAT_(varname, cap)

/**
 * Setter for the capability. It is adequate to access the capability name
 * without taking care of the suffixes. They use to go within a function
 * in charge of setting the capabilities.
 * @param varname name of the capability
 * @param val value of the capability
 */
#define WRITE_CAP(varname, val) CAT_(varname, cap) = (val)

/**
 * Disables a capability. It is adequate to access the capability name
 * without taking care of the suffixes. They used to go within a function
 * in charge of setting the capabilities.
 * @param varname name of the capability
 */
#define DISABLE_CAP(varname) CAT_(varname, cap) = 0

/**
 * Enables a capability. It is adequate to access the capability name
 * without taking care of the suffixes. They used to go within a function
 * in charge of setting the capabilities.
 * @param varname name of the capability
 */
#define ENABLE_CAP(varname) CAT_(varname, cap) = 1

/**
 * Enables an operation capability. It is adequate to access the capability
 * name without taking care of the suffixes. They used to go within a function
 * in charge of setting the capabilities.
 * @param varname name of the capability
 */
#define ENABLE_OPERATION(varname, op) \
  CAT_(varname, cap) = CAT_(varname, cap) | (op)

/**
 * Setter for a range capability. It is adequate to access the capability name
 * without taking care of the suffixes. They use to go within a function
 * in charge of setting the capabilities.
 * @param varname name of the capability
 * @param min_val minimum value of the capability
 * @param max_val maximum value of the capability
 */
#define WRITE_RANGE_CAP(varname, min_val, max_val) \
  CAT_(varname, min) = (min_val);                  \
  CAT_(varname, max) = (max_val)

/**
 * Accessor for a range capability. An accessor goes to a function when passing
 * parameters in a function call.
 * @param varname name of the range capability
 */
#define ACCESS_CAP_RANGE(varname) CAT_(varname, min), CAT_(varname, max)

/**
 * Accessor for minimum of a range capability. It gets access to the variable's
 * value.
 * @param varname name of the range capability
 */
#define ACCESS_CAP_MIN(varname) CAT_(varname, min)

/**
 * Accessor for maximum of a range capability. It gets access to the variable's
 * value.
 * @param varname name of the range capability
 */
#define ACCESS_CAP_MAX(varname) CAT_(varname, max)

/**
 * Accessor for a simple capability. It gets access to the variable's value.
 * @param varname name of the range capability
 */
#define ACCESS_CAP(varname) CAT_(varname, cap)

/**
 * Accesor for the parameter. It is adequate to access the parameter name
 * without taking care of the suffixes. They use to go within a function
 * in charge of setting the parameter. It can be used for accessing the
 * value of a given parameter from the user.
 * @param varname name of the parameter
 */
#define READ_EXE_PARAM(varname) CAT_(varname, wr)

/**
 * Setter for the parameter. It is adequate to access the parameter name
 * without taking care of the suffixes. It can be used for setting the
 * value of a given parameter to the user.
 * @param varname name of the parameter
 * @param val value to set
 */
#define WRITE_EXE_PARAM(varname, val) CAT_(varname, rd) = (val)

/**
 * Accesor for the parameter. Equivalent to READ_EXE_PARAM but in an inverse
 * direction. It is adequate for testbenches
 * @param varname name of the parameter
 */
#define READ_EXE_PARAM_TB(varname) CAT_(varname, rd)

/**
 * Accesor for the parameter. Equivalent to WRITE_EXE_PARAM but in an inverse
 * direction. It is adequate for testbenches
 * @param varname name of the parameter
 * @param val value to set
 */
#define WRITE_EXE_PARAM_TB(varname, val) CAT_(varname, wr) = (val)

/**
 * Accessor for a parameter. They go to function calls.
 * @param varname name of the parameter
 */
#define ACCESS_EXE_PARAM(varname) CAT_(varname, rd), CAT_(varname, wr)

/**
 * Verifies if a param has a value (checks equality). If the value is
 * different to the value hold by the param, it fallsback to the given
 * fallback value
 * @param param parameter name
 * @param value value to check
 * @param fallback value to fallback if the check fails
 */
#define CHECK_EQUALITY_AND_FALLBACK(param, value, fallback) \
  {                                                         \
    auto actual_param_value = READ_EXE_PARAM(param);        \
    if ((value) != actual_param_value) {                    \
      WRITE_EXE_PARAM(param, (fallback));                   \
    } else {                                                \
      WRITE_EXE_PARAM(param, actual_param_value);           \
    }                                                       \
  }

/**
 * Verifies if a param has a value within a range. If the value is
 * outside of the range, it fallsback to the given fallback value
 * @param param parameter name
 * @param value_min minimum value to check
 * @param value_max minimum value to check
 * @param fallback value to fallback if the check fails
 */
#define CHECK_RANGE_AND_FALLBACK(param, value_min, value_max, fallback) \
  {                                                                     \
    auto actual_param_value = READ_EXE_PARAM(param);                    \
    if ((value_min) >= actual_param_value ||                            \
        (value_max) <= actual_param_value) {                            \
      WRITE_EXE_PARAM(param, (fallback));                               \
    } else {                                                            \
      WRITE_EXE_PARAM(param, actual_param_value);                       \
    }                                                                   \
  }

/**
 * Declare param as not used. This avoids leaving ports dangling and
 * disconnected.
 * @param param parameter name
 */
#define DECLARE_AS_NOT_USED(param) \
  WRITE_EXE_PARAM(param, READ_EXE_PARAM(param));
