/*
 * Copyright 2022
 * Author: David Cordero Chavarría <dacoch215@estudiantec.cr>
 */

#include "axc_multiply.hpp"

void axc_multiply_accel_top(const DataType a, const DataType b, DataType& c) {
  axc_multiply(a, b, c);
}
