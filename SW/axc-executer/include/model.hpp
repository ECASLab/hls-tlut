/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <array>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include <executer.hpp>
#include <layer.hpp>
#include <runtime.hpp>

class Model {
 public:
  /**
   * @brief Construct a new Model object
   *
   * The model will be uninitialised
   */
  Model() = default;

  /**
   * @brief Destroy the Model object
   */
  virtual ~Model() = default;

  /**
   * @brief Loads the model and its parameter from the binary files
   *
   * @param path folder to the model weights
   * @param paths files with the weights for each layer
   * @return Runtime
   */
  virtual Runtime Load(const std::string &path,
                       const std::vector<std::string> &paths) = 0;

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
  virtual Runtime Execute(std::shared_ptr<Executer> executer,
                          std::shared_ptr<DataContainer> input,
                          std::shared_ptr<DataContainer> output) = 0;

 protected:
  /**
   * @brief Layers of the model in a sequential way
   */
  std::vector<std::shared_ptr<BasicLayer>> layers_;

  /**
   * @brief Function to read the weights of a layer provided the layer
   * configurations
   *
   * @param filename name to the path to read
   * @param layer layer configuration. It must define the expected size
   * BasicLayer::size
   * @return Runtime
   */
  Runtime ReadWeights(const std::filesystem::path &filename, BasicLayer *layer);
};
