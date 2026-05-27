/*
 * Copyright (C) 2023
 * Authors:
 *        Luis G. Leon Vega <luis.leon@ieee.org>
 */

#pragma once

#include <layer.hpp>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "executers/mqaccelerators.hpp"

/*
 * This is a configuration file to determine each layer configuration
 * and how the units must be implemented
 */

/**
 * @brief Quantisation parameters
 * This is specific for the layers
 */
static constexpr std::pair<int, int> kQT[] = {
    {Q_CONV_BW, Q_CONV_INT},   /* Conv 2D */
    {Q_ADD2D_BW, Q_ADD2D_INT}, /* Add 2D*/
    {Q_ADD2D_BW, Q_ADD2D_INT}, /* Not needed */
    {Q_CONV_BW, Q_CONV_INT},   /* Conv 2D */
    {Q_ADD2D_BW, Q_ADD2D_INT}, /* Add 2D */
    {Q_ADD2D_BW, Q_ADD2D_INT}, /* Not needed */
    {Q_DENSE_BW, Q_DENSE_INT}, /* Dense */
    {Q_ADD1D_BW, Q_ADD1D_INT}, /* Add 1D */
    {Q_DENSE_BW, Q_DENSE_INT}, /* Dense */
    {Q_ADD1D_BW, Q_ADD1D_INT}, /* Add 1D */
    {Q_DENSE_BW, Q_DENSE_INT}, /* Dense */
    {Q_ADD1D_BW, Q_ADD1D_INT}, /* Add 1D */
    {20, 10}                    /* Softmax */ //MODIFICADO!
};

/**
 * @brief Accelerator configurations
 *
 * This defines all the configurations of the accelerators to implement
 *
 * This defines the executions unit to generate in order to fullfil the
 * requirements of each layer
 */
static constexpr std::tuple<Layers, DataBits, IntBits, DropBits, DropBits,
                            ArithApprox>
    kAccelConfigs[] = {
        {Layers::CONV2D, 10, 2, 2, 2, ArithApprox::EXACT_A},
        {Layers::CONV2D, 12, 4, 2, 1, ArithApprox::EXACT_A},
        {Layers::DENSE, 12, 6, 1, 1, ArithApprox::EXACT_A},
        {Layers::DENSE, 14, 6, 1, 1, ArithApprox::EXACT_A},
        {Layers::ADD, 12, 4, 1, 1, ArithApprox::EXACT_A},
        {Layers::ADD, 14, 6, 1, 1, ArithApprox::EXACT_A},
        {Layers::AVG_POOL, 12, 4, 1, 1, ArithApprox::EXACT_A},
        {Layers::REDUCT_ACTIVATION, 20, 10, 1, 1, ArithApprox::EXACT_A}, //MODIFICADO
};

/**
 * @brief Accelerator factory
 *
 * This creates the accelerators and puts them into an array that
 * will be used by the MQExecuter.
 *
 * Key: layer index
 * Value: function to invoke the accelerator
 */
static const std::vector<std::shared_ptr<IAccelerator>> MQAccelerators = {
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
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[6]), std::get<1>(kAccelConfigs[6]),
        std::get<2>(kAccelConfigs[6]), std::get<3>(kAccelConfigs[6]),
        std::get<4>(kAccelConfigs[6]), std::get<5>(kAccelConfigs[6])>>(
        kAccelConfigs[6]),
    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[7]), std::get<1>(kAccelConfigs[7]),
        std::get<2>(kAccelConfigs[7]), std::get<3>(kAccelConfigs[7]),
        std::get<4>(kAccelConfigs[7]), std::get<5>(kAccelConfigs[7])>>(
        kAccelConfigs[7]),
};
