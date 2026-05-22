/*
 * Copyright 2021
 * Author: Luis G. Leon-Vega <lleon95@estudiantec.cr>
 */

#include "convolver_vector.hpp"

/* The convolution happens in row-major but it is transferred by columns */
void convolver_vector_top_accel(DataType input[kRowsWindow][kWindowSize],
                                DataType kernel[kKernelSize][kKernelSize],
                                DataType output[kRowsOutput][kOutputSize]) {
#pragma HLS INLINE
  using Engine = ama::hw::convolvers::Spatial<DataType, Q_K, Q_O>;
  ama::hw::ParallelConvolver<Q_ACCEL, Q_ACCEL, Engine>::Execute(input, kernel,
                                                                output);
}
