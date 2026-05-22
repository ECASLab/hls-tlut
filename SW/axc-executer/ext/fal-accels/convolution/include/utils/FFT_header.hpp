/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */
#pragma once
/* FFT template */

template <typename T>
using Complex = std::complex<T>;

template <typename T>
using CArray = std::valarray<Complex<T>>;

#define kPI 3.141592653589
