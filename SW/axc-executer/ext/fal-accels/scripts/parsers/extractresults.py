#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import re
import sys

import pandas as pd
from enum import Enum


class Tests(Enum):
    MEAN = 1
    STD = 2
    MIN = 3
    MAX = 4
    MSE = 5
    RMSE = 6
    PSNR = 7
    SSIM = 8
    HISTOGRAM_PROCESSED = 9,
    FROBENIUS = 10,
    HISTOMIN = 11,
    HISTOMAX = 12,
    HISTOBINS = 13


TestStrings = {
    Tests.MEAN: 'Mean',
    Tests.STD: 'Std',
    Tests.MIN: 'Min',
    Tests.MAX: 'Max',
    Tests.MSE: 'MSE',
    Tests.RMSE: 'RMSE',
    Tests.PSNR: 'PSNR',
    Tests.SSIM: 'SSIM',
    Tests.HISTOGRAM_PROCESSED: 'Histogram',
    Tests.HISTOMIN: 'HistMin',
    Tests.HISTOMAX: 'HistMax',
    Tests.HISTOBINS: 'HistBins',
    Tests.FROBENIUS: 'Frobenius'
}

TestParser = {
    Tests.MEAN: lambda x: float(x),
    Tests.STD: lambda x: float(x),
    Tests.MIN: lambda x: float(x),
    Tests.MAX: lambda x: float(x),
    Tests.MSE: lambda x: float(x),
    Tests.RMSE: lambda x: float(x),
    Tests.PSNR: lambda x: float(x),
    Tests.SSIM: lambda x: float(x),
    Tests.HISTOGRAM_PROCESSED: lambda x: list(map(float, x.replace('[', '').replace(']', '').split(';'))),
    Tests.FROBENIUS: lambda x: float(x),
    Tests.HISTOMIN: lambda x: float(x),
    Tests.HISTOMAX: lambda x: float(x),
    Tests.HISTOBINS: lambda x: int(x),
}


def extract_results(file, tests=list(TestStrings.keys())):
    '''
    Reads the file and retrieve the tests into a dictionary

    Parameters
    ----------
    file: string
        File to read
    tests: list
        List of tests based on #Tests

    Returns
    -------
    result: dictionary
        Dictionary with the tests as keys and the results as the values.
        The structure is the following: keys from #TestStrings and values
        after applying the parsers from #TestParser
    '''
    lines = open(file).readlines()
    result = {}

    for test in tests:
        for line in lines:
            teststr = TestStrings[test]
            if re.match(teststr, line):
                val = TestParser[test](line.split(':')[1])
                key = line.split(':')[0]
                result[key] = val
    return result


if __name__ == "__main__":
    tests = list(TestStrings.keys())
    file = sys.argv[1]
    print("Parsing file:", file)
    results = extract_results(file, tests)
    results2 = extract_results(file, tests)
    print("Results:", results)
    df = pd.DataFrame.from_dict([results2, results])
    print(df)
