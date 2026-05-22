#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import sys

from xml.etree import ElementTree as ET


def fill_data(file):
    report = {}

    # Get the tree
    tree = ET.parse(file)
    root = tree.getroot()
    if root is None:
        print("Cannot load data")
        return

    for p in root.findall('.//UserAssignments'):
        report["part"] = p.find('Part').text
        report["frequency"] = 1000 / float(p.find('TargetClockPeriod').text)

    # Get latency
    for p in root.findall('.//SummaryOfOverallLatency'):
        report["latency_min"] = int(p.find('Best-caseLatency').text)
        report["latency_max"] = int(p.find('Worst-caseLatency').text)
        report["latency_avg"] = int(p.find('Average-caseLatency').text)

    # Get resources
    for p in root.findall('.//Resources'):
        report["resources_dsp"] = int(p.find('DSP48E').text)
        report["resources_lut"] = int(p.find('LUT').text)
        report["resources_bram"] = int(p.find('BRAM_18K').text)
        report["resources_ff"] = int(p.find('FF').text)

    # Get available resources
    for p in root.findall('.//AvailableResources'):
        report["available_dsp"] = int(p.find('DSP48E').text)
        report["available_lut"] = int(p.find('LUT').text)
        report["available_bram"] = int(p.find('BRAM_18K').text)
        report["available_ff"] = int(p.find('FF').text)

    # Get frequency
    for p in root.findall('.//SummaryOfTimingAnalysis'):
        report["latency_period"] = float(p.find('EstimatedClockPeriod').text)
        report["latency_frequency"] = 1000. / report["latency_period"]
        report["latency_avgtime"] = report["latency_avg"] * report["latency_period"]
        report["latency_mintime"] = report["latency_min"] * report["latency_period"]
        report["latency_maxtime"] = report["latency_max"] * report["latency_period"]

    return report


def extract_consumption(file):
    '''
    Reads the file and retrieve the resources and latency into a dictionary

    Parameters
    ----------
    file: string
        File to read

    Returns
    -------
    result: dictionary
        Dictionary with the resources, latency, and available as
        keys and the results as the values. The structure is the following:
        {"latency": {"period", "frequency", "mintime", "maxtime", "avgtime",
                     "min", "max", "avg"},
         "resources": {"dsp", "bram", "ff", "lut"},
         "available": {"dsp", "bram", "ff", "lut"},
         "relative": {"dsp", "bram", "ff", "lut"}}
    '''
    result = fill_data(file)

    return result

if __name__ == "__main__":
    file = sys.argv[1]
    print("Parsing file:", file)
    results = extract_consumption(file)
    print("Results:", results)
