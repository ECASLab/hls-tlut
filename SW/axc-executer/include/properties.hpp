/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <string>

/**
 * @brief Custom Execution Property
 */
enum class CustomProps {
  /**
   * @brief Data width in bits for quantisation types
   */
  Q_DATA_WIDTH,
  /**
   * @brief Integer width in bits for quantisation types
   */
  Q_INTEGER_WIDTH,
};

/**
 * @brief Layer Execution Properties
 *
 * This struct holds values for custom execution properties that are required
 * for executers. For instance, to determine the integer width and data width
 * in a more agnostic fashion.
 *
 */
struct CustomPropsPayload {
  enum Type { INT = 0, FLOAT, STR };

  /** Data type hold by this structure */
  Type type;

  /** Integer payload */
  int intp;

  /** Float payload */
  float floatp;

  /** String payload */
  std::string strp;
};
