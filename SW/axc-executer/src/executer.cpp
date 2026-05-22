/*
 * Copyright (C) 2022
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#include <executer.hpp>
#include <kernel.hpp>
#include <kernels/arithmetic.hpp>

#include <iostream>

#define DEBUG_MODE 1

Runtime Executer::Conv2D(const std::shared_ptr<BasicLayer> ilayer,
                         const std::shared_ptr<DataContainer> input,
                         std::shared_ptr<DataContainer> output) {
  return Kernels::Conv2D<float>(ilayer.get(), input.get(), output.get());
}

Runtime Executer::DepthConv2D(const std::shared_ptr<BasicLayer> ilayer,
                              const std::shared_ptr<DataContainer> input,
                              std::shared_ptr<DataContainer> output) {
  return Kernels::DepthConv2D<float>(ilayer.get(), input.get(), output.get());
}

Runtime Executer::Dense(const std::shared_ptr<BasicLayer> ilayer,
                        const std::shared_ptr<DataContainer> input,
                        std::shared_ptr<DataContainer> output) {
  return Kernels::Dense<float>(ilayer.get(), input.get(), output.get());
}

Runtime Executer::Add(const std::shared_ptr<BasicLayer> ilayer,
                      const std::shared_ptr<DataContainer> input,
                      std::shared_ptr<DataContainer> output) {
  using arithm = axc::arithmetic::exact::Add<float>;
  using kernel =
      Kernels::Exact::ElementWise<float, Kernels::NonLinear::STL<float>,
                                  arithm>;

  return Kernels::ElementWise<float, kernel, ::Add>(ilayer.get(), input.get(),
                                                    output.get());
}

Runtime Executer::Multiplier(const std::shared_ptr<BasicLayer> ilayer,
                             const std::shared_ptr<DataContainer> input,
                             std::shared_ptr<DataContainer> output) {
  using arithm = axc::arithmetic::exact::Multiply<float>;
  using kernel =
      Kernels::Exact::ElementWise<float, Kernels::NonLinear::STL<float>,
                                  arithm>;

  return Kernels::ElementWise<float, kernel, ::Multiplier>(
      ilayer.get(), input.get(), output.get());
}

Runtime Executer::Add2D(const std::shared_ptr<BasicLayer> ilayer,
                        const std::shared_ptr<DataContainer> input,
                        std::shared_ptr<DataContainer> output) {
  using arithm = axc::arithmetic::exact::Add<float>;
  using kernel =
      Kernels::Exact::ElementWise2D<float, Kernels::NonLinear::STL<float>,
                                    arithm>;

  return Kernels::ElementWise2D<float, kernel, ::Add>(ilayer.get(), input.get(),
                                                      output.get());
}

Runtime Executer::Multiplier2D(const std::shared_ptr<BasicLayer> ilayer,
                               const std::shared_ptr<DataContainer> input,
                               std::shared_ptr<DataContainer> output) {
  using arithm = axc::arithmetic::exact::Multiply<float>;
  using kernel =
      Kernels::Exact::ElementWise2D<float, Kernels::NonLinear::STL<float>,
                                    arithm>;

  return Kernels::ElementWise2D<float, kernel, ::Multiplier>(
      ilayer.get(), input.get(), output.get());
}

Runtime Executer::Pooling(const std::shared_ptr<BasicLayer> ilayer,
                          const std::shared_ptr<DataContainer> input,
                          std::shared_ptr<DataContainer> output) {
  return Kernels::Pooling<float>(ilayer.get(), input.get(), output.get());
}

Runtime Executer::Padding(const std::shared_ptr<BasicLayer> ilayer,
                          const std::shared_ptr<DataContainer> input,
                          std::shared_ptr<DataContainer> output) {
  return Kernels::Padding2D<float>(ilayer.get(), input.get(), output.get());
}

Runtime Executer::Layerwise(
    const std::shared_ptr<BasicLayer> ilayer,
    const std::vector<std::shared_ptr<DataContainer>> inputs,
    std::shared_ptr<DataContainer> output) {
  using NonLinear = Kernels::NonLinear::STL<float>;
  using LayerType = Layer<typename ::Layerwise>;

  Runtime ret;

  if (inputs.size() == 0) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Cannot process a layer without inputs"};
  }

  DataContainer **inputs_array = new DataContainer *[inputs.size()];
  for (decltype(inputs.size()) i = 0; i < inputs.size(); ++i) {
    inputs_array[i] = const_cast<DataContainer *>(inputs.at(i).get());
  }
  auto const_inputs_array = const_cast<const DataContainer **>(inputs_array);

  auto layer = std::dynamic_pointer_cast<const LayerType>(ilayer);
  if (!layer) {
    return Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                   "Incompatible layer in Layerwise input"};
  }

  Layers operation = layer->props.operation;
  switch (operation) {
    case Layers::ADD: {
      using arithm = axc::arithmetic::exact::Add<float>;
      using kernel = Kernels::Exact::Layerwise<float, NonLinear, arithm>;
      ret = Kernels::Layerwise<float, kernel>(ilayer.get(), const_inputs_array,
                                              output.get());
    }; break;
    case Layers::MULTIPLIER: {
      using arithm = axc::arithmetic::exact::Multiply<float>;
      using kernel = Kernels::Exact::Layerwise<float, NonLinear, arithm>;
      ret = Kernels::Layerwise<float, kernel>(ilayer.get(), const_inputs_array,
                                              output.get());
    }; break;
    default:
      ret = Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                    "Unsupported operation in layerwise"};
      break;
  }

  delete[] inputs_array;
  return ret;
}

Runtime Executer::Activation(const std::shared_ptr<BasicLayer> ilayer,
                             const std::shared_ptr<DataContainer> input,
                             std::shared_ptr<DataContainer> output) {
  return Kernels::Mapper<float>(ilayer.get(), input.get(), output.get());
}

Runtime Executer::ReductionActivation(
    const std::shared_ptr<BasicLayer> ilayer,
    const std::shared_ptr<DataContainer> input,
    std::shared_ptr<DataContainer> output) {
  return Kernels::ReductionActivation<float>(ilayer.get(), input.get(),
                                             output.get());
}

Runtime Executer::RunLayer(const std::shared_ptr<BasicLayer> ilayer,
                           std::shared_ptr<DataContainer> input,
                           std::shared_ptr<DataContainer> output) {
  Runtime ret{0, "Successful"};

  if (!ilayer) {
    return Runtime{Runtime::INVALID_PARAMETER, "Missing layer pointer"};
  }

  auto type = ilayer->type;
  int data_width = 0;
  int integer_width = 0;

  /* Find the data and integer width when using ap_fixed */
  const auto dit = ilayer->custom_props.find(CustomProps::Q_DATA_WIDTH);
  if (dit != ilayer->custom_props.end()) {
    data_width = dit->second.intp;
  }
  const auto iit = ilayer->custom_props.find(CustomProps::Q_INTEGER_WIDTH);
  if (iit != ilayer->custom_props.end()) {
    integer_width = iit->second.intp;
  }

  /* Lambda to look for compatibilities in accesting accelerators */
  auto layer_comp = [&](const auto &accel) {
    const AccelConfig &config = accel->GetConfigs();
    bool pass = true;

    /* Check the type */
    pass &= std::get<0>(config) == type;
    pass &= std::get<1>(config) == data_width;
    pass &= std::get<2>(config) == integer_width;
    /* TODO: Add support for other properties */

    return pass;
  };

  /* Search for compatibility */
  auto compatible =
      std::find_if(accelerators_.begin(), accelerators_.end(), layer_comp);

  auto PrintDimensions_ = [](auto container) {
    if (!container) return;
#ifdef DEBUG_MODE
    std::cout << "\t Dimensions: ";
    for (int i = 0; i < container->num_dimensions; ++i) {
      std::cout << container->dimensions[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "\t Samples: ";
    int limit = (container->size >> 2) < 5 ? (container->size >> 2) : 5;
    for (int i = 0; i < limit; ++i) {
      std::cout << container->GetData()[i] << " ";
    }
    std::cout << std::endl;
#endif
  };

  /* Get the input in case */
  auto GetInput_ = [&](auto ilayer, auto input) {
    decltype(input) input_, qinput_;
    if (!input) {
      const auto &input_layers = ilayer->input_layers;
      if (input_layers.size() != ilayer->num_inputs) {
        std::cerr << "There is a mismatch between the number of inputs while "
                     "running layer"
                  << std::endl;
        input_ = std::shared_ptr<DataContainer>{};
        qinput_ = ilayer->GetContainer(input_);
      }
      if (input_layers.size() == 0) {
        std::cerr << "Cannot deal with layers with no inputs" << std::endl;
        return std::shared_ptr<DataContainer>{};
      }
      input_ = input_layers[0]->output;
      qinput_ = ilayer->GetContainer(input_);
      input_layers[0]->output = qinput_;
    } else {
      input_ = input;
      qinput_ = ilayer->GetContainer(input_);
    }

    PrintDimensions_(qinput_);
    return qinput_;
  };

  /* Get the inputs in the case of multiple */
  auto GetInputs_ = [&](auto ilayer) {
    std::vector<decltype(input)> input_vec_;
    const auto &input_layers = ilayer->input_layers;

    if (input_layers.size() != ilayer->num_inputs) {
      std::cerr << "There is a mismatch between the number of inputs while "
                   "running layer"
                << std::endl;
      return input_vec_;
    }

    if (input_layers.size() == 0) {
      std::cerr << "Cannot deal with layers with no inputs" << std::endl;
      return input_vec_;
    }

    for (decltype(input_layers.size()) i = 0; i < input_layers.size(); ++i) {
      auto input_ = input_layers[i]->output;
      PrintDimensions_(input_);
      auto qinput = ilayer->GetContainer(input_);
      input_vec_.push_back(qinput);
      input_layers[i]->output = qinput;
    }

    return input_vec_;
  };

  /* Get the output from internally */
  auto GetOutput_ = [&](auto ilayer, auto output) {
    decltype(output) output_;
    output_ = !output ? std::make_shared<DataContainer>() : output;
    output_ = ilayer->GetContainer(output_);
    ilayer->output = output_;
    return output_;
  };

  if (accelerators_.end() != compatible) {
#ifdef DEBUG_MODE
    std::cout << "Using Custom Accelerator Kernel" << std::endl;
#endif
    auto input_ = GetInput_(ilayer, input);
    auto output_ = GetOutput_(ilayer, output);
    return (*compatible)->Execute(ilayer, input_, output_);
  }

  /* Not supported, thus, use parent and dequantisation */
#ifdef DEBUG_MODE
  std::cout << "Using SW Kernel: " << LayerNames[static_cast<int>(ilayer->type)]
            << std::endl;
#endif

  switch (type) {
    case Layers::CONV2D: {
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      ret = Conv2D(ilayer, input_, output_);
    }; break;
    case Layers::DEPTHWISE_CONV2D: {
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      ret = DepthConv2D(ilayer, input_, output_);
    }; break;
    case Layers::PADDING: {
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      ret = Padding(ilayer, input_, output_);
    }; break;
    case Layers::DENSE: {
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      ret = Dense(ilayer, input_, output_);
    }; break;
    case Layers::MULTIPLIER: {
      using LayerType = Layer<typename ::Multiplier>;
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      auto layer = std::dynamic_pointer_cast<const LayerType>(ilayer);
      Layers preceding = layer->props.preceding;

      switch (preceding) {
        case Layers::CONV2D:
          ret = Multiplier2D(ilayer, input_, output_);
          break;
        case Layers::DENSE:
          ret = Multiplier(ilayer, input_, output_);
          break;
        default:
          ret = Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                        "Unsupported layer preceding an multiplier"};
          break;
      }
    }; break;
    case Layers::ADD: {
      using LayerType = Layer<typename ::Add>;
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      auto layer = std::dynamic_pointer_cast<const LayerType>(ilayer);
      Layers preceding = layer->props.preceding;

      switch (preceding) {
        case Layers::CONV2D:
          ret = Add2D(ilayer, input_, output_);
          break;
        case Layers::DENSE:
          ret = Add(ilayer, input_, output_);
          break;
        default:
          ret = Runtime{Runtime::INCOMPATIBLE_PARAMETER,
                        "Unsupported layer preceding an addition"};
          break;
      }
    }; break;
    case Layers::MAX_POOL:
    case Layers::AVG_POOL: {
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      ret = Pooling(ilayer, input_, output_);
    }; break;
    case Layers::ACTIVATION: {
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      ret = Activation(ilayer, input_, output_);
    }; break;
    case Layers::REDUCT_ACTIVATION: {
      auto input_ = GetInput_(ilayer, input);
      auto output_ = GetOutput_(ilayer, output);
      ret = ReductionActivation(ilayer, input_, output_);
    }; break;
    case Layers::LAYERWISE: {
      auto inputs_ = GetInputs_(ilayer);
      auto output_ = GetOutput_(ilayer, output);
      ret = Layerwise(ilayer, inputs_, output_);
    }; break;
    default:
      return Runtime{
          Runtime::INCOMPATIBLE_PARAMETER,
          "Layer " + std::to_string(static_cast<int>(type)) + " not supported"};
  }

  return ret;
}

Runtime Executer::Run(const std::vector<std::shared_ptr<BasicLayer>> &layers,
                      std::shared_ptr<DataContainer> input,
                      std::shared_ptr<DataContainer> output) {
  Runtime ret{Runtime::OK, "Successful"};

  int counter = 0;

  auto num_layers = layers.size();

  if (num_layers == 1) {
    ret = RunLayer(layers[0], input, nullptr);
    if (ret.code) {
      std::cerr << "Error while executing layer Nº " << counter
                << " with error: " << ret.msg << std::endl;
      return ret;
    }
  } else {
    /* Run the first layer */
    ret = RunLayer(layers[0], input, nullptr);
    if (ret.code) {
      std::cerr << "Error while executing layer Nº " << counter
                << " with error: " << ret.msg << std::endl;
      return ret;
    }

    /* Run the layers in the middle */
    for (uint i = 1; i < (num_layers - 1); ++i) {
      auto layer = layers[i];
      ret = RunLayer(layer, nullptr, nullptr);
      ++counter;
      if (ret.code) {
        std::cerr << "Error while executing layer Nº " << counter
                  << " with error: " << ret.msg << std::endl;
        return ret;
      }
    }

    /* Run the last layer */
    ret = RunLayer(layers[num_layers - 1], nullptr, nullptr);
    if (ret.code) {
      std::cerr << "Error while executing layer Nº " << counter
                << " with error: " << ret.msg << std::endl;
      return ret;
    }
  }

  /* Write on the proper output and steal the reference */
  if (Runtime::OK == ret.code && output) {
    auto &output_layer_data = layers[num_layers - 1]->output;
    output->SetData(output_layer_data->GetData());
    output->size = output_layer_data->size;
    output->num_dimensions = output_layer_data->num_dimensions;

    for (int i = 0; i < output->num_dimensions; ++i)
      output->dimensions[i] = output_layer_data->dimensions[i];

    output_layer_data->SetData(nullptr);
  }

  return ret;
}
