############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

from string import Template

def open_template(file: str):
    '''
    Opens the template to fill

    Parameters
    ----------
    file: string
        template location

    Returns
    -------
    tmpl : string
        template
    '''

    tmpl = ""

    with open(file, 'r') as f:
        tmpl = f.read()

    return tmpl

def reconstruct_layers(params):
    '''
    Reconstruct the layer configurations (quantisation)

    Parameters
    ----------
    params: list of list
        List of convolution layers[0] and dense layers[1]

    Returns
    -------
    layer_configurations: list of strings
        list of configurations
    '''
    layers = []
    # Reconstruct the main layers -> three per element
    convlayers = params[0]
    denselayers = params[1]

    # Reconstruct the conv layers
    for layer in convlayers:
        layers.append("    {" + f"{layer['BW']}, {layer['IW']}" + "}") # clayer
        layers.append("    {" + f"{layer['BW']}, {layer['IW']}" + "}") # alayer
        layers.append("    {" + f"{layer['BW']}, {layer['IW']}" + "}") # player

    # Reconstruct the dense layers
    for layer in denselayers:
        layers.append("    {" + f"{layer['BW']}, {layer['IW']}" + "}") # dlayer
        layers.append("    {" + f"{layer['BW']}, {layer['IW']}" + "}") # alayer
    
    # Reconstruct the softmax layer
    layer = denselayers[-1]
    layers.append("    {" + f"{layer['BW']}, {layer['IW']}" + "}") # slayer

    return layers

def reconstruct_accel_configs(params):
    '''
    Reconstruct the layer configurations (quantisation)

    Parameters
    ----------
    params: list of list
        List of convolution layers[0] and dense layers[1]

    Returns
    -------
    layer_configurations: list of strings
        list of configurations
    '''
    configlayers = []
    # Reconstruct the main layers -> three per element
    convlayers = params[0]
    denselayers = params[1]

    # Reconstruct the conv layers
    for layer in convlayers:
        CONVART = layer['ART'].split(',')[0]
        ADDART = layer['ART'].split(',')[1]
        # clayer
        configlayers.append("    {" + f"Layers::CONV2D, {layer['BW']}, {layer['IW']}, {layer['DBA']}, {layer['DBM']}, ArithApprox::{CONVART}_A" + "}")
        # alayer
        configlayers.append("    {" + f"Layers::ADD, {layer['BW']}, {layer['IW']}, {layer['DBA']}, {layer['DBM']}, ArithApprox::{ADDART}_A" + "}")
        # player
        configlayers.append("    {" + f"Layers::AVG_POOL, {layer['BW']}, {layer['IW']}, {layer['DBA']}, {layer['DBM']}, ArithApprox::{ADDART}_A" + "}")

    # Reconstruct the dense layers
    for layer in denselayers:
        DENSEART = layer['ART'].split(',')[0]
        ADDART = layer['ART'].split(',')[1]
        # dlayer
        configlayers.append("    {" + f"Layers::DENSE, {layer['BW']}, {layer['IW']}, {layer['DBA']}, {layer['DBM']}, ArithApprox::{DENSEART}_A" + "}")
        # alayer
        configlayers.append("    {" + f"Layers::ADD, {layer['BW']}, {layer['IW']}, {layer['DBA']}, {layer['DBM']}, ArithApprox::{ADDART}_A" + "}")
    
    # Softmax is not accelerated
    return configlayers

def reconstruct_accels(num_accels):
    '''
    Reconstruct the accels in a factory

    Parameters
    ----------
    num_accels: int
        Number of accels to construct
    
    Returns
    -------
    accels: list
        List of accels
    '''
    accels = []

    for i in range(num_accels):
        accel = f"""    std::make_shared<Accelerator<
        std::get<0>(kAccelConfigs[{i}]), std::get<1>(kAccelConfigs[{i}]),
        std::get<2>(kAccelConfigs[{i}]), std::get<3>(kAccelConfigs[{i}]),
        std::get<4>(kAccelConfigs[{i}]), std::get<5>(kAccelConfigs[{i}])>>()"""
        accels.append(accel)
    
    return accels

def fill_template(file: str, params):
    tmpl_str = open_template(file)
    tmpl = Template(tmpl_str)

    qt = reconstruct_layers(params)
    accel_configs = reconstruct_accel_configs(params)
    accels = reconstruct_accels(len(accel_configs))

    s = {
        'layers': ',\n'.join(qt),
        'accel_configs': ',\n'.join(accel_configs),
        'accels': ',\n'.join(accels),
    }

    result = tmpl.substitute(s)
    return result