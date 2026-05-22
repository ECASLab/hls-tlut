/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <accelerator.hpp>
#include <layer.hpp>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "examples/mqlenet5/executers/mqaccelerators.hpp"

/*
 * Quantisation parameters
 * This is specific for the layers
 * This include contains the quantisations for all layers
 */
// clang-format off
#include "model/quantisations.hpp"
// clang-format on

using AccelConfigs =
    std::tuple<Layers, DataBits, IntBits, DropBits, DropBits, ArithApprox>;

/*
 * Macro to simplify the declaration of the accelerators
 */
#define MAKE_ACCELERATOR(cfg)                                          \
  std::make_shared<Accelerator<std::get<0>((cfg)), std::get<1>((cfg)), \
                               std::get<2>((cfg)), std::get<3>((cfg)), \
                               std::get<4>((cfg)), std::get<5>((cfg))>>((cfg))

/**
 * @brief Accelerator configurations
 *
 * This defines all the configurations of the accelerators to implement
 *
 * This defines the executions unit to generate in order to fullfil the
 * requirements of each layer
 */
static constexpr AccelConfigs kAccelConfigs[] = {
    {Layers::REDUCT_ACTIVATION, 16, 6, 1, 1, ArithApprox::EXACT_A},
};

/* Overrides the default */
static std::vector<std::shared_ptr<IAccelerator>> MQAccelerators = {};

/**
 * @brief Accelerator factory
 *
 * This creates the accelerators and puts them into an array that
 * will be used by the Executer.
 *
 * Key: layer index
 * Value: function to invoke the accelerator
 */
static std::vector<std::shared_ptr<IAccelerator>> mobilenetv2_accels = {
    MAKE_ACCELERATOR(kAccelConfigs[0]),
};
