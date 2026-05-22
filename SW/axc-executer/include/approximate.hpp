/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <string>
#include <unordered_map>

/**
 * @brief Defines the approximation for the arithmetic
 *
 * The _A suffix is to avoid conflicts with macros
 */
enum class ArithApprox {
  EXACT_A = 0, /** Use exact arithmetic */
  LSBOR_A,     /** Use LSB OR-ing */
  LSBDROP_A,   /** Use LSB Dropping */
  LSBFIXED_A   /** Use LSB Fixed */
};

/**
 * @brief Defines the approximation description for stringification
 *
 * This helps to stringify the name of the approximation for printing
 */
static std::unordered_map<int, std::string> ArithApproxStr = {
    {static_cast<int>(ArithApprox::EXACT_A), "Exact"},
    {static_cast<int>(ArithApprox::LSBOR_A), "LSBOR"},
    {static_cast<int>(ArithApprox::LSBDROP_A), "LSBDROP"},
    {static_cast<int>(ArithApprox::LSBFIXED_A), "LSBFIXED"},
};
