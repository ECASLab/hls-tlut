/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include <executer.hpp>
#include <layer.hpp>
#include <model.hpp>
#include <runtime.hpp>

class LeNet5 : public Model {
 public:
  /**
   * @brief Construct a new LeNet5 object
   */
  LeNet5();

  /**
   * @brief Destroy the LeNet5 object
   */
  ~LeNet5();

  /**
   * @brief Number of layers of the LeNet5
   *
   */
  static constexpr unsigned int num_layers = 13;

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
