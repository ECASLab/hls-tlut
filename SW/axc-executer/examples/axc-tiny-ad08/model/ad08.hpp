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

#include "examples/axc-tiny-ad08/config.hpp"

static constexpr int kNumLayers = 22;

/**
 * @brief AXC AD08 Class
 *
 * It implements a multi-quantisation lenet class. It is able to perform
 * layer computations with multiple quantisation types
 *
 */
class AD08 : public Model {
 public:
  AD08();

  /**
   * @brief Defines the QTuple type
   *
   * The first element corresponds to the data width and the second to the
   * integer part
   *
   */
  typedef std::pair<int, int> QTuple;

  /**
   * @brief Destroy the AD08 object
   */
  ~AD08() = default;

  /**
   * @brief Number of layers of the AD08
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

inline AD08::AD08() {
  /* Allocate Layer objects */
  layers_ = {
      std::make_shared<QLayer<ap_fixed<kQT[0].first, kQT[0].second>, Dense>>(),
      std::make_shared<QLayer<ap_fixed<kQT[1].first, kQT[1].second>, Add>>(),
      std::make_shared<
          QLayer<ap_fixed<kQT[2].first, kQT[2].second>, Multiplier>>(),
      std::make_shared<QLayer<ap_fixed<kQT[3].first, kQT[3].second>, Add>>(),

      std::make_shared<QLayer<ap_fixed<kQT[4].first, kQT[4].second>, Dense>>(),
      std::make_shared<QLayer<ap_fixed<kQT[5].first, kQT[5].second>, Add>>(),
      std::make_shared<
          QLayer<ap_fixed<kQT[6].first, kQT[6].second>, Multiplier>>(),
      std::make_shared<QLayer<ap_fixed<kQT[7].first, kQT[7].second>, Add>>(),

      std::make_shared<QLayer<ap_fixed<kQT[8].first, kQT[8].second>, Dense>>(),
      std::make_shared<QLayer<ap_fixed<kQT[9].first, kQT[9].second>, Add>>(),
      std::make_shared<
          QLayer<ap_fixed<kQT[10].first, kQT[10].second>, Multiplier>>(),
      std::make_shared<QLayer<ap_fixed<kQT[11].first, kQT[11].second>, Add>>(),

      std::make_shared<
          QLayer<ap_fixed<kQT[12].first, kQT[12].second>, Dense>>(),
      std::make_shared<QLayer<ap_fixed<kQT[13].first, kQT[13].second>, Add>>(),
      std::make_shared<
          QLayer<ap_fixed<kQT[14].first, kQT[14].second>, Multiplier>>(),
      std::make_shared<QLayer<ap_fixed<kQT[15].first, kQT[15].second>, Add>>(),

      std::make_shared<
          QLayer<ap_fixed<kQT[16].first, kQT[16].second>, Dense>>(),
      std::make_shared<QLayer<ap_fixed<kQT[17].first, kQT[17].second>, Add>>(),
      std::make_shared<
          QLayer<ap_fixed<kQT[18].first, kQT[18].second>, Multiplier>>(),
      std::make_shared<QLayer<ap_fixed<kQT[19].first, kQT[19].second>, Add>>(),

      std::make_shared<
          QLayer<ap_fixed<kQT[20].first, kQT[20].second>, Dense>>(),
      std::make_shared<QLayer<ap_fixed<kQT[21].first, kQT[21].second>, Add>>(),
  };

  /* Define each layer characteristics */
  auto layer_0 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[0]);
  layer_0->activation = Activations::NONE;
  layer_0->type = Layers::DENSE;
  layer_0->size = 64 * 72 * sizeof(float);
  layer_0->input_size = 64;
  layer_0->output_size = 72;
  layer_0->data = nullptr;
  layer_0->props = Dense{.num_dimensions = 2, .dimensions = {64, 72}};
  layer_0->num_inputs = 1;
  layer_0->num_outputs = 1;

  auto layer_1 = std::dynamic_pointer_cast<Layer<Add>>(layers_[1]);
  layer_1->activation = Activations::NONE;
  layer_1->type = Layers::ADD;
  layer_1->size = 72 * sizeof(float);
  layer_1->input_size = -1;
  layer_1->output_size = -1;
  layer_1->data = nullptr;
  layer_1->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_1->num_inputs = 1;
  layer_1->num_outputs = 1;

  auto layer_2 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[2]);
  layer_2->activation = Activations::NONE;
  layer_2->type = Layers::MULTIPLIER;
  layer_2->size = 72 * sizeof(float);
  layer_2->data = nullptr;
  layer_2->input_size = -1;
  layer_2->output_size = -1;
  layer_2->props = Multiplier{.num_dimensions = 1, .dimensions = {72}};
  layer_2->num_inputs = 1;
  layer_2->num_outputs = 1;

  auto layer_3 = std::dynamic_pointer_cast<Layer<Add>>(layers_[3]);
  layer_3->activation = Activations::RELU;
  layer_3->type = Layers::ADD;
  layer_3->size = 72 * sizeof(float);
  layer_3->input_size = -1;
  layer_3->output_size = -1;
  layer_3->data = nullptr;
  layer_3->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_3->num_inputs = 1;
  layer_3->num_outputs = 1;

  auto layer_4 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[4]);
  layer_4->activation = Activations::NONE;
  layer_4->type = Layers::DENSE;
  layer_4->size = 72 * 72 * sizeof(float);
  layer_4->input_size = 72;
  layer_4->output_size = 72;
  layer_4->data = nullptr;
  layer_4->props = Dense{.num_dimensions = 2, .dimensions = {72, 72}};
  layer_4->num_inputs = 1;
  layer_4->num_outputs = 1;

  auto layer_5 = std::dynamic_pointer_cast<Layer<Add>>(layers_[5]);
  layer_5->activation = Activations::NONE;
  layer_5->type = Layers::ADD;
  layer_5->size = 72 * sizeof(float);
  layer_5->input_size = -1;
  layer_5->output_size = -1;
  layer_5->data = nullptr;
  layer_5->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_5->num_inputs = 1;
  layer_5->num_outputs = 1;

  auto layer_6 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[6]);
  layer_6->activation = Activations::NONE;
  layer_6->type = Layers::MULTIPLIER;
  layer_6->size = 72 * sizeof(float);
  layer_6->data = nullptr;
  layer_6->input_size = -1;
  layer_6->output_size = -1;
  layer_6->props = Multiplier{.num_dimensions = 1, .dimensions = {72}};
  layer_6->num_inputs = 1;
  layer_6->num_outputs = 1;

  auto layer_7 = std::dynamic_pointer_cast<Layer<Add>>(layers_[7]);
  layer_7->activation = Activations::RELU;
  layer_7->type = Layers::ADD;
  layer_7->size = 72 * sizeof(float);
  layer_7->input_size = -1;
  layer_7->output_size = -1;
  layer_7->data = nullptr;
  layer_7->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_7->num_inputs = 1;
  layer_7->num_outputs = 1;

  auto layer_8 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[8]);
  layer_8->activation = Activations::NONE;
  layer_8->type = Layers::DENSE;
  layer_8->size = 72 * 8 * sizeof(float);
  layer_8->input_size = 72;
  layer_8->output_size = 8;
  layer_8->data = nullptr;
  layer_8->props = Dense{.num_dimensions = 2, .dimensions = {72, 8}};
  layer_8->num_inputs = 1;
  layer_8->num_outputs = 1;

  auto layer_9 = std::dynamic_pointer_cast<Layer<Add>>(layers_[9]);
  layer_9->activation = Activations::NONE;
  layer_9->type = Layers::ADD;
  layer_9->size = 8 * sizeof(float);
  layer_9->input_size = -1;
  layer_9->output_size = -1;
  layer_9->data = nullptr;
  layer_9->props =
      Add{.num_dimensions = 1, .dimensions = {8}, .preceding = Layers::DENSE};
  layer_9->num_inputs = 1;
  layer_9->num_outputs = 1;

  auto layer_10 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[10]);
  layer_10->activation = Activations::NONE;
  layer_10->type = Layers::MULTIPLIER;
  layer_10->size = 8 * sizeof(float);
  layer_10->data = nullptr;
  layer_10->input_size = -1;
  layer_10->output_size = -1;
  layer_10->props = Multiplier{.num_dimensions = 1, .dimensions = {8}};
  layer_10->num_inputs = 1;
  layer_10->num_outputs = 1;

  auto layer_11 = std::dynamic_pointer_cast<Layer<Add>>(layers_[11]);
  layer_11->activation = Activations::RELU;
  layer_11->type = Layers::ADD;
  layer_11->size = 8 * sizeof(float);
  layer_11->input_size = -1;
  layer_11->output_size = -1;
  layer_11->data = nullptr;
  layer_11->props =
      Add{.num_dimensions = 1, .dimensions = {8}, .preceding = Layers::DENSE};
  layer_11->num_inputs = 1;
  layer_11->num_outputs = 1;

  auto layer_12 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[12]);
  layer_12->activation = Activations::NONE;
  layer_12->type = Layers::DENSE;
  layer_12->size = 8 * 72 * sizeof(float);
  layer_12->input_size = 8;
  layer_12->output_size = 72;
  layer_12->data = nullptr;
  layer_12->props = Dense{.num_dimensions = 2, .dimensions = {8, 72}};
  layer_12->num_inputs = 1;
  layer_12->num_outputs = 1;

  auto layer_13 = std::dynamic_pointer_cast<Layer<Add>>(layers_[13]);
  layer_13->activation = Activations::NONE;
  layer_13->type = Layers::ADD;
  layer_13->size = 72 * sizeof(float);
  layer_13->input_size = -1;
  layer_13->output_size = -1;
  layer_13->data = nullptr;
  layer_13->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_13->num_inputs = 1;
  layer_13->num_outputs = 1;

  auto layer_14 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[14]);
  layer_14->activation = Activations::NONE;
  layer_14->type = Layers::MULTIPLIER;
  layer_14->size = 72 * sizeof(float);
  layer_14->data = nullptr;
  layer_14->input_size = -1;
  layer_14->output_size = -1;
  layer_14->props = Multiplier{.num_dimensions = 1, .dimensions = {72}};
  layer_14->num_inputs = 1;
  layer_14->num_outputs = 1;

  auto layer_15 = std::dynamic_pointer_cast<Layer<Add>>(layers_[15]);
  layer_15->activation = Activations::RELU;
  layer_15->type = Layers::ADD;
  layer_15->size = 72 * sizeof(float);
  layer_15->input_size = -1;
  layer_15->output_size = -1;
  layer_15->data = nullptr;
  layer_15->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_15->num_inputs = 1;
  layer_15->num_outputs = 1;

  auto layer_16 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[16]);
  layer_16->activation = Activations::NONE;
  layer_16->type = Layers::DENSE;
  layer_16->size = 72 * 72 * sizeof(float);
  layer_16->input_size = 72;
  layer_16->output_size = 72;
  layer_16->data = nullptr;
  layer_16->props = Dense{.num_dimensions = 2, .dimensions = {72, 72}};
  layer_16->num_inputs = 1;
  layer_16->num_outputs = 1;

  auto layer_17 = std::dynamic_pointer_cast<Layer<Add>>(layers_[17]);
  layer_17->activation = Activations::NONE;
  layer_17->type = Layers::ADD;
  layer_17->size = 72 * sizeof(float);
  layer_17->input_size = -1;
  layer_17->output_size = -1;
  layer_17->data = nullptr;
  layer_17->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_17->num_inputs = 1;
  layer_17->num_outputs = 1;

  auto layer_18 = std::dynamic_pointer_cast<Layer<Multiplier>>(layers_[18]);
  layer_18->activation = Activations::NONE;
  layer_18->type = Layers::MULTIPLIER;
  layer_18->size = 72 * sizeof(float);
  layer_18->data = nullptr;
  layer_18->input_size = -1;
  layer_18->output_size = -1;
  layer_18->props = Multiplier{.num_dimensions = 1, .dimensions = {72}};
  layer_18->num_inputs = 1;
  layer_18->num_outputs = 1;

  auto layer_19 = std::dynamic_pointer_cast<Layer<Add>>(layers_[19]);
  layer_19->activation = Activations::RELU;
  layer_19->type = Layers::ADD;
  layer_19->size = 72 * sizeof(float);
  layer_19->input_size = -1;
  layer_19->output_size = -1;
  layer_19->data = nullptr;
  layer_19->props =
      Add{.num_dimensions = 1, .dimensions = {72}, .preceding = Layers::DENSE};
  layer_19->num_inputs = 1;
  layer_19->num_outputs = 1;

  auto layer_20 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[20]);
  layer_20->activation = Activations::NONE;
  layer_20->type = Layers::DENSE;
  layer_20->size = 72 * 64 * sizeof(float);
  layer_20->input_size = 72;
  layer_20->output_size = 64;
  layer_20->data = nullptr;
  layer_20->props = Dense{.num_dimensions = 2, .dimensions = {72, 64}};
  layer_20->num_inputs = 1;
  layer_20->num_outputs = 1;

  auto layer_21 = std::dynamic_pointer_cast<Layer<Add>>(layers_[21]);
  layer_21->activation = Activations::NONE;
  layer_21->type = Layers::ADD;
  layer_21->size = 64 * sizeof(float);
  layer_21->input_size = -1;
  layer_21->output_size = -1;
  layer_21->data = nullptr;
  layer_21->props =
      Add{.num_dimensions = 1, .dimensions = {64}, .preceding = Layers::DENSE};
  layer_21->num_inputs = 1;
  layer_21->num_outputs = 1;

  /* Connect layers */
  layer_0->Connect({}, {layer_1});
  layer_1->Connect({layer_0}, {layer_2});
  layer_2->Connect({layer_1}, {layer_3});
  layer_3->Connect({layer_2}, {layer_4});
  layer_4->Connect({layer_3}, {layer_5});
  layer_5->Connect({layer_4}, {layer_6});
  layer_6->Connect({layer_5}, {layer_7});
  layer_7->Connect({layer_6}, {layer_8});
  layer_8->Connect({layer_7}, {layer_9});
  layer_9->Connect({layer_8}, {layer_10});
  layer_10->Connect({layer_9}, {layer_11});
  layer_11->Connect({layer_10}, {layer_12});
  layer_12->Connect({layer_11}, {layer_13});
  layer_13->Connect({layer_12}, {layer_14});
  layer_14->Connect({layer_13}, {layer_15});
  layer_15->Connect({layer_14}, {layer_16});
  layer_16->Connect({layer_15}, {layer_17});
  layer_17->Connect({layer_16}, {layer_18});
  layer_18->Connect({layer_17}, {layer_19});
  layer_19->Connect({layer_18}, {layer_20});
  layer_20->Connect({layer_19}, {layer_21});
  layer_21->Connect({layer_20}, {});
}

inline Runtime AD08::Load(const std::string &path,
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

inline Runtime AD08::Execute(std::shared_ptr<Executer> executer,
                             std::shared_ptr<DataContainer> input,
                             std::shared_ptr<DataContainer> output) {
  if (!executer) {
    return Runtime{Runtime::INVALID_PARAMETER, "Executer is null"};
  }

  return executer->Run(layers_, input, output);
}
