/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <ap_fixed.h>
#include <executer.hpp>
#include <layer.hpp>
#include <model.hpp>
#include <runtime.hpp>

#include "examples/axc-mobilenetv2/config.hpp"

static constexpr int kNumLayers = 209;

/**
 * @brief AXC MobileNetV2 Class
 *
 * It implements a multi-quantisation MobileNetV2 class. It is able to perform
 * layer computations with multiple quantisation types
 *
 */
class MobileNetV2 : public Model {
 public:
  MobileNetV2();

  /**
   * @brief Defines the QTuple type
   *
   * The first element corresponds to the data width and the second to the
   * integer part
   *
   */
  typedef std::pair<int, int> QTuple;

  /**
   * @brief Destroy the MobileNetV2 object
   */
  ~MobileNetV2() = default;

  /**
   * @brief Number of layers of the MobileNetV2
   *
   */
  static constexpr unsigned int num_layers = kNumLayers;

  /**
   * @brief Loads the model and its parameter from the binary files
   *
   * @param path folder to the model weights
   * @param paths files with the weights for each layer
   * @return Runtime
   */
  Runtime Load(const std::string &path,
               const std::vector<std::string> &paths) override;

  /**
   * @brief Execute the model provided an executor.
   *
   * It performs the inference on an input buffer and provides an output buffer
   *
   * @param executer executor engine
   * @param input input buffer
   * @param output output buffer
   * @return Runtime
   */
  Runtime Execute(std::shared_ptr<Executer> executer,
                  std::shared_ptr<DataContainer> input,
                  std::shared_ptr<DataContainer> output) override;
};

MobileNetV2::MobileNetV2() {
/* Allocate Layer objects - All is automatically generated */
#include "vector.hpp"

/* Define each layer characteristics */
#include "definition.hpp"

/* Connect layers */
#include "connection.hpp"
}

inline Runtime MobileNetV2::Load(const std::string &path,
                                 const std::vector<std::string> &paths) {
  std::stringstream msg_ss;

  if (paths.size() != layers_.size()) {
    msg_ss << "Sizes between the paths and layers do not match"
           << " Paths: " << paths.size() << " Layers: " << layers_.size();
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER, msg_ss.str()};
  }

  for (unsigned int i = 0; i < layers_.size(); ++i) {
    Runtime res;
    if (layers_[i]->size == 0) {
      continue;
    }
    res = ReadWeights(path + "/" + paths[i], layers_[i].get());
    if (res.code) {
      return res;
    }
    res = layers_[i]->Quantise();
    if (res.code) {
      return res;
    }
  }

  msg_ss << num_layers << " layers read";
  return Runtime{0, msg_ss.str()};
}

inline Runtime MobileNetV2::Execute(std::shared_ptr<Executer> executer,
                                    std::shared_ptr<DataContainer> input,
                                    std::shared_ptr<DataContainer> output) {
  if (!executer) {
    return Runtime{Runtime::INVALID_PARAMETER, "Executer is null"};
  }

  return executer->Run(layers_, input, output);
}
