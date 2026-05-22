#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import json
import os
import sys
import time

from itertools import product

def read_file(file):
    '''
    Reads the JSON file

    Parameters
    ----------
    file: string
        String of the path to the JSON file

    Returns
    -------
    json: dictionary
        Dictionary with the JSON file structure
    '''
    data = None
    with open(file, 'r') as f:
        data = json.load(f)
    return data

def get_seed():
    '''
    Gets a seed for random numbers

    Returns
    -------
    seed: integer
        Seed as an integer number depend on time
    '''
    return int(int(time.time()) % 1e6)
    
def get_config(file):
    '''
    Gets the configuration from the JSON file

    Parameters
    ----------
    file: string
        String of the path to the JSON file

    Returns
    -------
    json: dictionary
        Dictionary with the JSON file structure including a seed
    '''
    jsondict = read_file(file)

    # Seed processing
    if (not 'Q_SEED' in jsondict.keys()):
        jsondict['Q_SEED'] = [get_seed()]
    
    # Paths processing
    if (jsondict.get("TB_ARGV")):
        jsondict["TB_ARGV"] = [os.path.abspath(i) for i in jsondict["TB_ARGV"]]
    
    # Config the datatype len
    qfixed = []
    for v in product(jsondict['Q_INT'], jsondict['Q_BW']):
        if v[0] / v[1] <= jsondict['Q_FIXED_RATIO']:
            qfixed.append({"Q_INT": v[0], "Q_BW": v[1]})
    
    jsondict["Q_FIXED"] = list(qfixed)
    jsondict.pop("Q_INT", None)
    jsondict.pop("Q_BW", None)
    jsondict.pop("Q_FIXED_RATIO", None)

    return jsondict

def get_permutations(config):
    '''
    Gets the permutations in a list of dictionaries for the runners

    Parameters
    ----------
    config: dictionary
        Dictionary with the possible values of environment variables
    
    Returns
    -------
    result: list of dictionaries
        List of dictionaries for the runners
    '''
    # Compute all the permutations
    result = [dict(zip(config, v)) for v in product(*config.values())]
   
    # Fix the fixed point step
    def expand_fixed_point(x):
        x["Q_INT"] = x["Q_FIXED"]["Q_INT"]
        x["Q_BW"] = x["Q_FIXED"]["Q_BW"]
        x.pop("Q_FIXED", None)
        return x

    filtered = map(expand_fixed_point, result)
    return list(filtered)

if __name__ == "__main__":
    conf = get_config(sys.argv[1])
    perms = get_permutations(conf)
    print(f"Total permutations: {len(perms)}")
    print("First perm:\n", perms[0])
