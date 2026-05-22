/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#ifndef __HLS_MATH_H__
#define __HLS_MATH_H__

#ifndef __cplusplus
#error C++ is required to include this header file
#else

/* Override classes in the meantime */
struct half;

namespace hls {
template <typename T>
T cos(const T op) {
  return op;
}

template <typename T>
T sin(const T op) {
  return op;
}
}  // namespace hls

#endif /* __cplusplus */
#endif /* __HLS_MATH_H__ */
