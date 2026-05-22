/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <string>

/**
 * @brief Structure to define the return characteristics of each function
 *
 * It includes a code and a description that works to track errors
 */
struct Runtime {
  /** Error codes */
  enum {
    OK = 0,            /** OK Status */
    FILE_ERROR,        /** File error that can be read or write */
    INVALID_PARAMETER, /** Invalid argument or parameter. i.e. nullptr */
    /** Incompatible parameter that it is not supported
                              by a function */
    INCOMPATIBLE_PARAMETER,
    NOT_IMPLEMENTED, /** Not implemented error */
  };

  int code;        /** Code of the error */
  std::string msg; /** Description of the error */

  /**
   * @brief Construct a new Runtime object
   *
   * It default the error to be 0 or OK
   */
  Runtime() noexcept : code{0} {}

  /**
   * @brief Construct a new Runtime object
   *
   * It defines the constructor to define a custom code and description
   *
   * @param code code of the error
   * @param msg description
   */
  Runtime(const int code, const std::string &msg) noexcept
      : code{code}, msg{msg} {}
};
