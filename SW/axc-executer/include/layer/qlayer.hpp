/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <data.hpp>
#include <layer/layer.hpp>
#include <properties.hpp>
#include <runtime.hpp>

#include <memory>

/**
 * @brief Quantised layer specialisation template
 *
 * It specialises the layer to content layer-specific properties. Moreover,
 * it quantises the layer
 * @tparam Q quantisation type. It must support conversion from float to the
 * target type
 * @tparam Props Class to the layer-specific properties
 */
template <class Q, class Props>
struct QLayer : public Layer<Props> {
  typedef Q QType; /** Requires to save the quantisation type */

  std::shared_ptr<Q> q_data{nullptr}; /** Quantised parameters data */
  bool is_quantised = false;          /** It is quantised */

  /**
   * @brief Quantises the Layer
   *
   * It quantises the weights and it is used when loading the parameters
   *
   * @return Runtime
   */
  Runtime Quantise() override;

  /**
   * @brief Get the Container object
   *
   * This quantises the incoming DataContainer to the correct type
   * compatible with the layer.
   *
   * The use of shared pointers is justified by the complexity of handling the
   * cases where:
   *
   * - The layer is not quantised: so, the reference must preserve without
   *   deallocation
   * - The layer is quantised and the buffer is compatible: the reference is
   *   also preserved
   * - The layer is quantised and the buffer is incompatible: dequantise and
   *   quantise again (or requantised)
   *
   * @param in incoming buffer to convert
   * @return DataContainer* [Full Transfer] output container. The user is in
   * charge of deallocating the buffer. When there is no quantisation, the
   * shared_ptr behaves like a reference
   */
  std::shared_ptr<DataContainer> GetContainer(
      std::shared_ptr<DataContainer> in) override;

  QLayer();

  virtual ~QLayer() = default;
};

template <class Q, class Props>
inline QLayer<Q, Props>::QLayer() : Layer<Props>{} {
  /* Set quantisation properties */
  this->custom_props[CustomProps::Q_DATA_WIDTH] = {
      .type = CustomPropsPayload::Type::INT, .intp = Q::width};
  this->custom_props[CustomProps::Q_INTEGER_WIDTH] = {
      .type = CustomPropsPayload::Type::INT, .intp = Q::iwidth};
}

template <class Q, class Props>
inline Runtime QLayer<Q, Props>::Quantise() {
  uint64_t elems = this->size / sizeof(float);
  q_data = std::shared_ptr<Q>(new Q[elems], [](Q *p) { delete[] p; });

  for (uint64_t i = 0; i < elems; ++i) {
    q_data.get()[i] = this->data.get()[i];
  }

  is_quantised = true;

  return Runtime{Runtime::OK, "Quantised"};
}

template <class Q, class Props>
inline std::shared_ptr<DataContainer> QLayer<Q, Props>::GetContainer(
    std::shared_ptr<DataContainer> in) {
  using QContainerType = QDataContainer<QType>;
  std::shared_ptr<QContainerType> out = nullptr;

  /* Check if the container is quantised */
  if (!in->quantised) {
    out = std::make_shared<QContainerType>(*in.get());
    return out;
  }

  /* Check if the incoming container has the same quantisation type
     If so, nothing to do and return it. Otherwise, requantise */
  out = std::dynamic_pointer_cast<QContainerType>(in);
  if (!out) {
    out = std::make_shared<QContainerType>(*in.get());
  }

  return out;
}
