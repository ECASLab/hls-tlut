/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>
#include <filesystem>

#include <properties.hpp>
#include <runtime.hpp>
#include <unordered_map>

/**
 * @brief DataAllocation type
 *
 * It summarises the type of allocator used. In this case, the only one is the
 * DataAllocation::HOST, that uses userptr.
 */
enum class DataAllocation { HOST = 0 };

/**
 * @brief DataContainer structure
 *
 * It compresses a buffer data with its respective metadata. It summarises
 * the type of allocator, the actual data, the size, and dimensionality.
 */
struct DataContainer {
  /**
   * @brief Maximum number of dimensions supported by buffers
   */
  static constexpr int kMaxNumDims = 5;

  /**
   * @brief Pointer to the data
   */
  mutable float *data = nullptr;

  /**
   * @brief Allocation type of the data
   */
  DataAllocation alloc;

  /**
   * @brief Size of the data in bytes
   */
  uint64_t size = 0;

  /**
   * @brief Number of dimensions that the buffer has
   */
  int num_dimensions;

  /**
   * @brief Number of elements per dimensions
   */
  int dimensions[kMaxNumDims];

  /**
   * @brief Custom execution props
   */
  std::unordered_map<CustomProps, CustomPropsPayload> props;

  /**
   * @brief Quantised flag. Indicates if the container is quantised or not
   */
  bool quantised = false;

  /**
   * @brief Function to read the buffer from a binary file
   *
   * @param filename name of the file to read from
   * @return Runtime
   */
  Runtime ReadFromFile(const std::filesystem::path &filename);

  /**
   * @brief Function to write the buffer to a binary file
   *
   * @param filename name of the file to write onto
   * @return Runtime
   */
  Runtime WriteToFile(const std::filesystem::path &filename);

  /**
   * @brief Quantisation function
   *
   * It performs the data quantisation. If the DataContainer does not
   * support this, it just return OK
   *
   * @return Runtime
   */
  virtual Runtime Quantise() const;

  /**
   * @brief Dequantisation function
   *
   * It performs the data dequantisation. If the DataContainer does not
   * support this, it just return OK
   *
   * @return Runtime
   */
  virtual Runtime Dequantise() const;

  /**
   * @brief Get the Data pointer
   *
   * @param nomove avoid moving memory during the transaction
   *
   * @return pointer to the float data
   */
  virtual float *GetData(const bool nomove = false);

  /**
   * @brief Get the Data pointer
   *
   * @param nomove avoid moving memory during the transaction
   *
   * @return pointer to the float data
   */
  virtual const float *GetData(const bool nomove = false) const;

  /**
   * @brief Set the Data pointer
   *
   * @param data data to set as pointer
   */
  virtual void SetData(float *data);

  /**
   * @brief Destroy the Data Container object. It deallocates the buffer in
   * case of using usrptr
   */
  virtual ~DataContainer();
};

inline Runtime DataContainer::Quantise() const {
  return Runtime{Runtime::OK, "Not implemented"};
}

inline Runtime DataContainer::Dequantise() const {
  return Runtime{Runtime::OK, "Not implemented"};
}

inline float *DataContainer::GetData(const bool nomove) { return this->data; }

inline const float *DataContainer::GetData(const bool nomove) const {
  return this->data;
}

inline void DataContainer::SetData(float *data) { this->data = data; }
