/*
 * Copyright 2022
 * Author: David Cordero Chavarría <dacoch215@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h> /* ap_fixed<N> type */
#include <ap_int.h>   /* ap_[u]int<N> types */

using DataType = ap_fixed<Q_BW, Q_INT>;

/**
 * Top function:
 * This declares the top function that wraps all the accelerator logic.
 * @param a [in]: first param
 * @param b [in]: second param
 * @param c [out]: third param
 */
void axc_multiply_accel_top(const DataType a, const DataType b, DataType& c);

/**
 * Multiply:
 * This declares the operation function that wraps all the processing logic. In
 * this case it performs a multiplication
 * @param a [in]: first param
 * @param b [in]: second param
 * @param c [out]: third param
 */
void axc_multiply(const DataType a, const DataType b, DataType& c);
