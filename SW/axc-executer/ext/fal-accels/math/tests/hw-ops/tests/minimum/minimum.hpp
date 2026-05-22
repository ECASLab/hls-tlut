/*
 * Copyright 2022
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

/**
 * Top function:
 * This declares the top function that wraps all the accelerator logic.
 * @param a [in]: first param
 * @param b [in]: second param
 * @param c [out]: third param
 */
void minimum_accel_top(const unsigned int a, const unsigned int b,
                       unsigned int& c);

/**
 * Multiply:
 * This declares the operation function that wraps all the processing logic. In
 * this case it performs a multiplication
 * @param a [in]: first param
 * @param b [in]: second param
 * @param c [out]: third param
 */
void multiply(const unsigned int a, const unsigned int b, unsigned int& c);
