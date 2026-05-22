/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "examples/mqlenet5/executers/mqaccelerators.hpp"

#include <layer.hpp>

/*
 * This is a configuration file to determine each layer configuration
 * and how the units must be implemented
 */

#define Q_INT_EXTERNAL_LAYER 12
#define Q_INT_INTERNAL_LAYER 6

/**
 * @brief Quantisation parameters
 * This is specific for the layers
 */
static constexpr std::pair<int, int> kQT[] = {
    {EXTERNAL_BW, Q_INT_EXTERNAL_LAYER}, /* Dense */
    {EXTERNAL_BW, Q_INT_EXTERNAL_LAYER}, /* Add */
    {EXTERNAL_BW, Q_INT_EXTERNAL_LAYER}, /* Mult */
    {EXTERNAL_BW, Q_INT_EXTERNAL_LAYER}, /* Add */

    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Dense */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Mult */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */

    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Dense */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Mult */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */

    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Dense */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Mult */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */

    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Dense */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Mult */
    {INTERNAL_BW, Q_INT_INTERNAL_LAYER}, /* Add */

    {EXTERNAL_BW, Q_INT_EXTERNAL_LAYER}, /* Dense */
    {EXTERNAL_BW, Q_INT_EXTERNAL_LAYER}, /* Add */
};

using AccelConfigs =
    std::tuple<Layers, DataBits, IntBits, DropBits, DropBits, ArithApprox>;

/**
 * @brief Accelerator configurations
 *
 * This defines all the configurations of the accelerators to implement
 *
 * This defines the executions unit to generate in order to fullfil the
 * requirements of each layer
 */
static constexpr AccelConfigs kAccelConfigs[] = {
    {Layers::ADD, EXTERNAL_BW, Q_INT_EXTERNAL_LAYER, 2, 2,
     ArithApprox::EXACT_A},
    {Layers::MULTIPLIER, EXTERNAL_BW, Q_INT_EXTERNAL_LAYER, 2, 2,
     ArithApprox::EXACT_A},
    {Layers::ADD, INTERNAL_BW, Q_INT_INTERNAL_LAYER, 4, 4,
     ArithApprox::EXACT_A},
    {Layers::MULTIPLIER, INTERNAL_BW, Q_INT_INTERNAL_LAYER, 4, 4,
     ArithApprox::EXACT_A},
    {Layers::DENSE, INTERNAL_BW, Q_INT_INTERNAL_LAYER, 1, 1,
     ArithApprox::EXACT_A},
    {Layers::DENSE, EXTERNAL_BW, Q_INT_EXTERNAL_LAYER, 1, 1,
     ArithApprox::EXACT_A},
};

/* Overrides the default */
static std::vector<std::shared_ptr<IAccelerator>> MQAccelerators = {};

/**
 * @brief Accelerator factory
 *
 * This creates the accelerators and puts them into an array that
 * will be used by the MQExecuter.
 *
 * Key: layer index
 * Value: function to invoke the accelerator
 */
static std::vector<std::shared_ptr<IAccelerator>> ad08accels = {
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[0]), std::get<1>(kAccelConfigs[0]),
        std::get<2>(kAccelConfigs[0]), std::get<3>(kAccelConfigs[0]),
        std::get<4>(kAccelConfigs[0]), std::get<5>(kAccelConfigs[0])>>(
        kAccelConfigs[0]),
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[1]), std::get<1>(kAccelConfigs[1]),
        std::get<2>(kAccelConfigs[1]), std::get<3>(kAccelConfigs[1]),
        std::get<4>(kAccelConfigs[1]), std::get<5>(kAccelConfigs[1])>>(
        kAccelConfigs[1]),
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[2]), std::get<1>(kAccelConfigs[2]),
        std::get<2>(kAccelConfigs[2]), std::get<3>(kAccelConfigs[2]),
        std::get<4>(kAccelConfigs[2]), std::get<5>(kAccelConfigs[2])>>(
        kAccelConfigs[2]),
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[3]), std::get<1>(kAccelConfigs[3]),
        std::get<2>(kAccelConfigs[3]), std::get<3>(kAccelConfigs[3]),
        std::get<4>(kAccelConfigs[3]), std::get<5>(kAccelConfigs[3])>>(
        kAccelConfigs[3]),
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[4]), std::get<1>(kAccelConfigs[4]),
        std::get<2>(kAccelConfigs[4]), std::get<3>(kAccelConfigs[4]),
        std::get<4>(kAccelConfigs[4]), std::get<5>(kAccelConfigs[4])>>(
        kAccelConfigs[4]),
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[5]), std::get<1>(kAccelConfigs[5]),
        std::get<2>(kAccelConfigs[5]), std::get<3>(kAccelConfigs[5]),
        std::get<4>(kAccelConfigs[5]), std::get<5>(kAccelConfigs[5])>>(
        kAccelConfigs[5]),
};
