#!/usr/bin/env python3
############################################################
# Copyright 2023
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import itertools

# ----------------- Design Space -----------------
# Convolution design space
ConvDS = {
    "BW" : [14],
    "IW" : [6],
    "DBA" : [1, 2, 3],
    "DBM" : [1, 2, 3],
    "ART" : ["EXACT,LSBFIXED", "EXACT,LSBOR", "EXACT,LSBDROP", "LSBFIXED,LSBFIXED"],
}
ConvEDS = {
    "BW" : [14, 16],
    "IW" : [6],
    "DBA" : [1],
    "DBM" : [1],
    "ART" : ["EXACT,EXACT"]
}
CONV_LAYERS=2

# Dense design space
DenseDS = {
    "BW" : [14],
    "IW" : [6],
    "DBA" : [1, 2, 3],
    "DBM" : [1, 2, 3],
    "ART" : ["EXACT,LSBFIXED", "EXACT,LSBOR", "EXACT,LSBDROP", "LSBFIXED,LSBFIXED"],
}
DenseEDS = {
    "BW" : [14, 16],
    "IW" : [6],
    "DBA" : [1],
    "DBM" : [1],
    "ART" : ["EXACT,EXACT"]
}
DENSE_LAYERS=3

# ----------------- Permutation factory -----------------
def get_ds_permutations(ds):
    keys, values = zip(*ds.items())
    experiments = [dict(zip(keys, v)) for v in itertools.product(*values)]
    return experiments

def get_permutations():
    '''
    The permutations are incremental to reduce the population
    '''
    # Create settings dictionary
    conv_configs = get_ds_permutations(ConvDS) + get_ds_permutations(ConvEDS)
    dense_configs = get_ds_permutations(DenseDS) + get_ds_permutations(DenseEDS)

    print(f"Convolution Config Permutations: {len(conv_configs)}")
    print(f"Dense Config Permutations: {len(dense_configs)}")

    print(f"Convolution layers to permute: {CONV_LAYERS}")
    print(f"Dense layers to permute: {DENSE_LAYERS}")

    if len(conv_configs) != len(dense_configs):
        print("Error: Number of configs is asymmetric")

    # Perform the dot product between layers and the configs
    conv_layers_configs = [[i] * CONV_LAYERS for i in conv_configs]
    dense_layers_configs = [[i] * DENSE_LAYERS for i in dense_configs]

    # Enable this to make all possible permutations
    #conv_layers_configs = list(itertools.product(conv_configs, repeat=CONV_LAYERS))
    #dense_layers_configs = list(itertools.product(dense_configs, repeat=DENSE_LAYERS))

    # Perform the cross-product between configurations
    configs = list(itertools.product(conv_layers_configs, dense_layers_configs))

    print(f"Total Permutations: {len(configs)}")

    return configs, CONV_LAYERS, DENSE_LAYERS
