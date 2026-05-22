#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import ast
import sys

import pandas as pd


def extract_errors(file):
    '''
    Reads the file and retrieve the differences into a dictionary

    Parameters
    ----------
    file: string
        File to read

    Returns
    -------
    result: dictionary
        Dictionary with the tests as keys and the results as the values.
        The key is identified as "Differences"
    '''
    lines = open(file).readlines()
    linestring = lines[0].replace("][", ", ")
    differences = ast.literal_eval(linestring)

    result = {"Differences": differences}
    return result


if __name__ == "__main__":
    file = sys.argv[1]
    print("Parsing file:", file)
    results = extract_errors(file)
    print("Results:", results)
    df = pd.DataFrame.from_dict([results])
    print(df)
