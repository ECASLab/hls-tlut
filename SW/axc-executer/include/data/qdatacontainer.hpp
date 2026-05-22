/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <algorithm>

#include <data/datacontainer.hpp>
#include <properties.hpp>
#include <runtime.hpp>

/**
 * @brief QDataContainer structure
 *
 * It compresses a buffer data with its respective metadata. It summarises
 * the type of allocator, the actual data, the size, and dimensionality. It
 * also contains quantised data
 *
 * @tparam Q quantised class
 */
template <class Q>
struct QDataContainer : public DataContainer {
  /**
   * @brief Data type accesor

   * This holds the quantisation data type for checking purposes
   */
  typedef Q QType;

  /**
   * @brief Flags to keep the coherency at the float memory level
   */
  mutable bool coherent_float = false;

  /**
   * @brief Flags to keep the coherency at the quantised memory level
   */
  mutable bool coherent_quantised = false;

  /**
   * @brief Construct a new QDataContainer object
   *
   * Default constructor
   */
  QDataContainer();

  /**
   * @brief Construct a new QDataContainer object
   *
   * This constructor receives a DataContainer and steals the pointer
   *
   * @param dc DataContainer object whose data will be stealed
   */
  explicit QDataContainer(DataContainer &dc);  // NOLINT

  /**
   * @brief Quantised data buffer
   */
  mutable Q *q_data = nullptr;

  /**
   * @brief Quantise the data from DataContainer::data to
   * QDataContainer<Q>::q_data
   *
   * @return Runtime
   */
  Runtime Quantise() const override;

  /**
   * @brief Dequantise the data from QDataContainer<Q>::q_data to
   * DataContainer::data. It provides compatibility with non-accelerated layers
   *
   * @return Runtime
   */
  Runtime Dequantise() const override;
  /**
   * @brief Get the Data pointer
   *
   * @param nomove avoid moving memory during the transaction
   *
   * @return pointer to the float data
   */
  float *GetData(const bool nomove = false) override;

  /**
   * @brief Get the Data pointer
   *
   * @param nomove avoid moving memory during the transaction
   *
   * @return pointer to the float data
   */
  const float *GetData(const bool nomove = false) const override;

  /**
   * @brief Set the Data pointer
   *
   * @param data data to set as pointer
   */
  void SetData(float *data) override;

  /**
   * @brief Get the Quantised Data pointer
   *
   * @param nomove avoid moving memory during the transaction
   *
   * @return pointer to the quantised data
   */
  Q *GetQData(const bool nomove = false);

  /**
   * @brief Get the Quantised Data pointer
   *
   * @param nomove avoid moving memory during the transaction
   *
   * @return pointer to the quantised data
   */
  const Q *GetQData(const bool nomove = false) const;

  /**
   * @brief Set the Quantised Data pointer
   *
   * @param data data to set as pointer
   */
  void SetQData(Q *data);

  virtual ~QDataContainer();
};

template <class Q>
inline QDataContainer<Q>::QDataContainer(DataContainer &dc)  // NOLINT
    : DataContainer{dc} {
  /* Ensure coherency */
  this->data = dc.GetData();

  /* Full-Transfer */
  dc.size = 0;
  dc.data = nullptr;
  this->coherent_float = true;
  this->quantised = true;
  /* Set quantisation properties */
  this->props[CustomProps::Q_DATA_WIDTH] = {
      .type = CustomPropsPayload::Type::INT, .intp = Q::width};
  this->props[CustomProps::Q_INTEGER_WIDTH] = {
      .type = CustomPropsPayload::Type::INT, .intp = Q::iwidth};
}

template <class Q>
inline QDataContainer<Q>::QDataContainer() : DataContainer{} {
  this->quantised = true;
  /* Set quantisation properties */
  this->props[CustomProps::Q_DATA_WIDTH] = {
      .type = CustomPropsPayload::Type::INT, .intp = Q::width};
  this->props[CustomProps::Q_INTEGER_WIDTH] = {
      .type = CustomPropsPayload::Type::INT, .intp = Q::iwidth};
}

template <class Q>
inline QDataContainer<Q>::~QDataContainer() {
  /* TODO: add support for other kinds of allocation */
  if (q_data) {
    delete[] q_data;
    q_data = nullptr;
  }
}

template <class Q>
inline float *QDataContainer<Q>::GetData(const bool nomove) {
  if (!coherent_float && !nomove) Dequantise();
  return this->data;
}

template <class Q>
inline const float *QDataContainer<Q>::GetData(const bool nomove) const {
  if (!coherent_float && !nomove) Dequantise();
  return this->data;
}

template <class Q>
inline void QDataContainer<Q>::SetData(float *data) {
  this->coherent_quantised = false;
  DataContainer::SetData(data);
}

template <class Q>
inline Q *QDataContainer<Q>::GetQData(const bool nomove) {
  if (!coherent_quantised && !nomove) Quantise();
  return this->q_data;
}

template <class Q>
inline const Q *QDataContainer<Q>::GetQData(const bool nomove) const {
  if (!coherent_quantised && !nomove) Quantise();
  return this->q_data;
}

template <class Q>
inline void QDataContainer<Q>::SetQData(Q *data) {
  this->coherent_float = false;
  this->q_data = data;
}

template <class Q>
inline Runtime QDataContainer<Q>::Quantise() const {
  uint64_t elems = this->size / sizeof(float);

  if (0 == this->size || !this->data) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Cannot quantise empty containers"};
  }

  /* Allocate new quantisation buffer if does not exist */
  if (!q_data) {
    q_data = new Q[elems];
  }

  /* INFO: The quantised datatype must be able to copy back and forth from
   * floats */
  std::copy(this->data, this->data + elems, this->q_data);
  this->coherent_quantised = true;
  return Runtime{Runtime::OK, "Quantised"};
}

template <class Q>
inline Runtime QDataContainer<Q>::Dequantise() const {
  uint64_t elems = this->size / sizeof(float);

  if (0 == this->size || !this->q_data) {
    return Runtime{Runtime::INVALID_PARAMETER,
                   "Cannot dequantise empty containers"};
  }

  /* Allocate new quantisation buffer if does not exist */
  if (!data) {
    this->data = new float[elems];
  }

  /* INFO: The quantised datatype must be able to copy back and forth from
   * floats */
  std::copy(this->q_data, this->q_data + elems, this->data);
  this->coherent_float = true;
  return Runtime{Runtime::OK, "Dequantised"};
}
