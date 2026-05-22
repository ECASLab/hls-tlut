/*
 * Copyright 2021
 * Author: Alejandro Rodriguez Figueroa <alejandrorf@estudiantec.cr>
 * Supervisor: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#pragma once

#include <ap_fixed.h>

#ifndef Q_KS
#define K 3
#else
#define K Q_KS
#endif

#ifndef Q_BW
#define Q_BW 8
#endif


#ifndef mRows
#define mRows 3
#endif

#ifndef nCols
#define nCols 3
#endif

using DataType = ap_fixed<Q_BW, 0>;
