/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "winograd.hpp"

void winograd_top_accel(DataType input[kRowsWindow][kWindowSize],
                        DataType kernel[kKernelSize][kKernelSize],
                        DataType output[kRowsOutput][kOutputSize]) {
  ama::hw::convolvers::Winograd<DataType, Q_K, Q_O> op;
  op.Execute(input, kernel, output);
}
