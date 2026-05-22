/*
 * Copyright 2023
 * Author: Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <ap_fixed.h> /* ap_fixed<N> type */

using DataType = ap_fixed<Q_BW, Q_INT>;

/*
 * Top function:
 * This declares the top function that wraps all the accelerator logic.
 * @param a [in]: first param
 * @param b [out]: second param
 */
void axc_nl_accel_top(const DataType a, DataType& b);

/*
 * Non-Linear Accel:
 * This declares the operation function that wraps all the processing logic. In
 * this case it performs a non-linear operation
 * @param a [in]: first param
 * @param b [out]: second param
 */
void axc_nl(const DataType a, DataType& b);
