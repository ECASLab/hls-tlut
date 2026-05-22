#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import re
import sys

from enum import Enum


class DoFs(Enum):
    DATATYPE_FRACTION = 1
    DATATYPE_INTEGER = 2
    DATATYPE_WIDTH = 3
    SEED = 4
    BIG_MATRIX_INPUTS = 5
    BIG_MATRIX_OUTPUTS = 6
    CORE = 7
    PE_COUNT = 8
    PE_COLUMNS = 9,
    PE_ROWS = 10,
    IMAGE_COLUMNS = 11,
    IMAGE_ROWS = 12,
    KERNEL_COLUMNS = 13,
    KERNEL_ROWS = 14,
    IMAGE_NAME = 15,
    EXECUTION_COUNTER = 16


DoFStrings = {
    DoFs.DATATYPE_FRACTION: 'Datatype Fraction',
    DoFs.DATATYPE_INTEGER: 'Datatype Integer',
    DoFs.DATATYPE_WIDTH: 'Datatype Width',
    DoFs.SEED: 'Seed',
    DoFs.BIG_MATRIX_INPUTS: 'Big Matrix Inputs',
    DoFs.BIG_MATRIX_OUTPUTS: 'Big Matrix Outputs',
    DoFs.CORE: 'Core',
    DoFs.PE_COUNT: 'PEs',
    DoFs.PE_COLUMNS: 'PE Columns',
    DoFs.PE_ROWS: 'PE Rows',
    DoFs.IMAGE_COLUMNS: 'Image Columns',
    DoFs.IMAGE_ROWS: 'Image Rows',
    DoFs.KERNEL_COLUMNS: 'Kernel Columns',
    DoFs.KERNEL_ROWS: 'Kernel Rows',
    DoFs.IMAGE_NAME: 'Image name',
    DoFs.EXECUTION_COUNTER: 'Execution Counter'
}

DoFParser = {
    DoFs.DATATYPE_FRACTION: lambda x: int(x),
    DoFs.DATATYPE_INTEGER: lambda x: int(x),
    DoFs.DATATYPE_WIDTH: lambda x: int(x),
    DoFs.SEED: lambda x: int(x),
    DoFs.BIG_MATRIX_INPUTS: lambda x: int(x),
    DoFs.BIG_MATRIX_OUTPUTS: lambda x: int(x),
    DoFs.CORE: lambda x: x.replace('\n', ''),
    DoFs.PE_COUNT: lambda x: int(x),
    DoFs.PE_COLUMNS: lambda x: int(x),
    DoFs.PE_ROWS: lambda x: int(x),
    DoFs.IMAGE_COLUMNS: lambda x: int(x),
    DoFs.IMAGE_ROWS: lambda x: int(x),
    DoFs.KERNEL_COLUMNS: lambda x: int(x),
    DoFs.KERNEL_ROWS: lambda x: int(x),
    DoFs.IMAGE_NAME: lambda x: x.replace('\n', ''),
    DoFs.EXECUTION_COUNTER: lambda x: int(x)
}


def extract_dofs(file, dofs=list(DoFStrings.keys())):
    '''
    Reads the file and retrieve the dofs into a dictionary

    Parameters
    ----------
    file: string
        File to read
    dofs: list
        List of dofs based on #Tests

    Returns
    -------
    result: dictionary
        Dictionary with the dofs as keys and the results as the values.
        The structure is the following: keys from #DoFStrings and values
        after applying the parsers from #DoFParser

    '''
    lines = open(file).readlines()
    result = {}

    for dof in dofs:
        for line in lines:
            dofstr = DoFStrings[dof]
            if re.match(dofstr, line):
                val = DoFParser[dof](line.split(':')[1])
                key = line.split(':')[0]
                result[key] = val
    return result


if __name__ == "__main__":
    tests = list(DoFStrings.keys())
    file = sys.argv[1]
    print("Parsing file:", file)
    results = extract_dofs(file, tests)
    print("Results:", results)
