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

/**
 * @brief QLenet Class
 *
 * @tparam Q Quantisation type
 */
template <class Q>
class QLeNet5 : public Model {
 public:
  /**
   * @brief Construct a new QLeNet5 object
   */
  QLeNet5();

  /**
   * @brief Destroy the QLeNet5 object
   */
  ~QLeNet5() = default;

  /**
   * @brief Number of layers of the QLeNet5
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

template <class Q>
inline QLeNet5<Q>::QLeNet5() {
  /* Allocate Layer objects */
  layers_ = {std::make_shared<QLayer<Q, Conv2D>>(),
             std::make_shared<QLayer<Q, Add>>(),
             std::make_shared<QLayer<Q, Pooling>>(),
             std::make_shared<QLayer<Q, Conv2D>>(),
             std::make_shared<QLayer<Q, Add>>(),
             std::make_shared<QLayer<Q, Pooling>>(),
             std::make_shared<QLayer<Q, Dense>>(),
             std::make_shared<QLayer<Q, Add>>(),
             std::make_shared<QLayer<Q, Dense>>(),
             std::make_shared<QLayer<Q, Add>>(),
             std::make_shared<QLayer<Q, Dense>>(),
             std::make_shared<QLayer<Q, Add>>(),
             std::make_shared<QLayer<Q, ReductionActivation>>()};

  /* Define each layer characteristics */
  auto layer_0 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[0]);
  layer_0->activation = Activations::NONE;
  layer_0->type = Layers::CONV2D;
  layer_0->size = 5 * 5 * 1 * 6 * sizeof(float); /* 5 * 5 * 1 * 6 */
  layer_0->data = nullptr;
  layer_0->input_size = -1;
  layer_0->output_size = -1;
  layer_0->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {5, 5, 1, 6}, /* {5, 5, 1, 6} */
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};
  layer_0->num_inputs = 1;
  layer_0->num_outputs = 1;

  auto layer_1 = std::dynamic_pointer_cast<Layer<Add>>(layers_[1]);
  layer_1->activation = Activations::TANH;
  layer_1->type = Layers::ADD;
  layer_1->size = 6 * sizeof(float);
  layer_1->input_size = -1;
  layer_1->output_size = -1;
  layer_1->data = nullptr;
  layer_1->props =
      Add{.num_dimensions = 1, .dimensions = {6}, .preceding = Layers::CONV2D};
  layer_1->num_inputs = 1;
  layer_1->num_outputs = 1;

  auto layer_2 = std::dynamic_pointer_cast<Layer<Pooling>>(layers_[2]);
  layer_2->activation = Activations::NONE;
  layer_2->type = Layers::AVG_POOL;
  layer_2->size = 0;
  layer_2->input_size = -1;
  layer_2->output_size = -1;
  layer_2->data = nullptr;
  layer_2->props =
      Pooling{.num_dimensions = 2, .poolings = {2, 2}, .strides = {2, 2}};
  layer_2->num_inputs = 1;
  layer_2->num_outputs = 1;

  auto layer_3 = std::dynamic_pointer_cast<Layer<Conv2D>>(layers_[3]);
  layer_3->activation = Activations::NONE;
  layer_3->type = Layers::CONV2D;
  layer_3->size = 5 * 5 * 6 * 16 * sizeof(float);
  layer_3->input_size = -1;
  layer_3->output_size = -1;
  layer_3->data = nullptr;
  layer_3->props = Conv2D{.num_dimensions = 4,
                          .dimensions = {5, 5, 6, 16},
                          .strides = {1, 1, 1, 1},
                          .padding = {0, 0, 0, 0},
                          .dilatations = {1, 1, 1, 1}};
  layer_3->num_inputs = 1;
  layer_3->num_outputs = 1;

  auto layer_4 = std::dynamic_pointer_cast<Layer<Add>>(layers_[4]);
  layer_4->activation = Activations::TANH;
  layer_4->type = Layers::ADD;
  layer_4->size = 16 * sizeof(float);
  layer_4->input_size = -1;
  layer_4->output_size = -1;
  layer_4->data = nullptr;
  layer_4->props =
      Add{.num_dimensions = 1, .dimensions = {16}, .preceding = Layers::CONV2D};
  layer_4->num_inputs = 1;
  layer_4->num_outputs = 1;

  auto layer_5 = std::dynamic_pointer_cast<Layer<Pooling>>(layers_[5]);
  layer_5->activation = Activations::NONE;
  layer_5->type = Layers::AVG_POOL;
  layer_5->size = 0;
  layer_5->input_size = -1;
  layer_5->output_size = 256;
  layer_5->data = nullptr;
  layer_5->props =
      Pooling{.num_dimensions = 2, .poolings = {2, 2}, .strides = {2, 2}};
  layer_5->num_inputs = 1;
  layer_5->num_outputs = 1;

  auto layer_6 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[6]);
  layer_6->activation = Activations::NONE;
  layer_6->type = Layers::DENSE;
  layer_6->size = 256 * 120 * sizeof(float);
  layer_6->input_size = 256;
  layer_6->output_size = 120;
  layer_6->data = nullptr;
  layer_6->props = Dense{.num_dimensions = 2, .dimensions = {256, 120}};
  layer_6->num_inputs = 1;
  layer_6->num_outputs = 1;

  auto layer_7 = std::dynamic_pointer_cast<Layer<Add>>(layers_[7]);
  layer_7->activation = Activations::TANH;
  layer_7->type = Layers::ADD;
  layer_7->size = 120 * sizeof(float);
  layer_7->input_size = 120;
  layer_7->output_size = 120;
  layer_7->data = nullptr;
  layer_7->props =
      Add{.num_dimensions = 1, .dimensions = {120}, .preceding = Layers::DENSE};
  layer_7->num_inputs = 1;
  layer_7->num_outputs = 1;

  auto layer_8 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[8]);
  layer_8->activation = Activations::NONE;
  layer_8->type = Layers::DENSE;
  layer_8->size = 120 * 84 * sizeof(float);
  layer_8->input_size = 120;
  layer_8->output_size = 84;
  layer_8->data = nullptr;
  layer_8->props = Dense{.num_dimensions = 2, .dimensions = {120, 84}};
  layer_8->num_inputs = 1;
  layer_8->num_outputs = 1;

  auto layer_9 = std::dynamic_pointer_cast<Layer<Add>>(layers_[9]);
  layer_9->activation = Activations::TANH;
  layer_9->type = Layers::ADD;
  layer_9->size = 84 * sizeof(float);
  layer_9->input_size = 84;
  layer_9->output_size = 84;
  layer_9->data = nullptr;
  layer_9->props =
      Add{.num_dimensions = 1, .dimensions = {84}, .preceding = Layers::DENSE};
  layer_9->num_inputs = 1;
  layer_9->num_outputs = 1;

  auto layer_10 = std::dynamic_pointer_cast<Layer<Dense>>(layers_[10]);
  layer_10->activation = Activations::NONE;
  layer_10->type = Layers::DENSE;
  layer_10->size = 84 * 10 * sizeof(float);
  layer_10->input_size = 84;
  layer_10->output_size = 10;
  layer_10->data = nullptr;
  layer_10->props = Dense{.num_dimensions = 2, .dimensions = {84, 10}};
  layer_10->num_inputs = 1;
  layer_10->num_outputs = 1;

  auto layer_11 = std::dynamic_pointer_cast<Layer<Add>>(layers_[11]);
  layer_11->activation = Activations::NONE;
  layer_11->type = Layers::ADD;
  layer_11->size = 10 * sizeof(float);
  layer_11->input_size = 10;
  layer_11->output_size = 10;
  layer_11->data = nullptr;
  layer_11->props =
      Add{.num_dimensions = 1, .dimensions = {10}, .preceding = Layers::DENSE};
  layer_11->num_inputs = 1;
  layer_11->num_outputs = 1;

  auto layer_12 =
      std::dynamic_pointer_cast<Layer<ReductionActivation>>(layers_[12]);
  layer_12->activation = Activations::SOFTMAX;
  layer_12->type = Layers::REDUCT_ACTIVATION;
  layer_12->size = 0;
  layer_12->input_size = -1;
  layer_12->output_size = -1;
  layer_12->data = nullptr;
  layer_12->num_inputs = 1;
  layer_12->num_outputs = 1;

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
  layer_12->Connect({layer_11}, {});
}

template <class Q>
inline Runtime QLeNet5<Q>::Load(const std::string &path,
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

template <class Q>
inline Runtime QLeNet5<Q>::Execute(std::shared_ptr<Executer> executer,
                                   std::shared_ptr<DataContainer> input,
                                   std::shared_ptr<DataContainer> output) {
  if (!executer) {
    return Runtime{Runtime::INVALID_PARAMETER, "Executer is null"};
  }

  return executer->Run(layers_, input, output);
}
