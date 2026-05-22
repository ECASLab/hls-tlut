/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "space.hpp"

#include "hw-ops/Space.hpp"

void space_top_accel(DataType input[Q_K + 1][Q_K + 1],
                     DataType kernel[Q_K][Q_K],
                     DataType output[Q_K - 1][Q_K - 1]) {
  ama::hw::convolvers::Spatial<DataType, Q_K> op{};
  op.Execute(input, kernel, output);
}
