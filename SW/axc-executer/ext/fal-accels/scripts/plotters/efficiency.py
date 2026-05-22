#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import numpy as np
import time

def compute_relative_consumption(df):
    '''
    Compute the relative consumption for each resource

    It will divide the resources consumed by the available in the FPGAs.

    Parameters
    ----------
    df: pandas dataframe
        Dataframe to compute and edit. It will add the "relative_" columns
        along with the maximum consumption and the occupancy (inverse of
        consumption)
    '''
    resources = ["dsp", "lut", "bram", "ff"]
    # Create relative resources
    for res in resources:
        df["relative_" + res] = df["resources_" + res] / df["available_" + res]
    # Compute the maximum amongst
    relatives = ["relative_" + i for i in resources]
    df["consumption"] = df[relatives].max(axis=1)
    df["occupancy"] = 1. / df["consumption"]
    df["occupancy"] = df["occupancy"].apply(np.ceil)


def compute_efficiency(df, equation, peak_performance):
    '''
    Computes the efficiency

    It parses the equation coming from the JSON file, calling the columns
    described by the JSON equation and performs a series of multiplications
    and additions. It generations the operations, the performance and
    the efficiency figure of metric. It also introduces the ideal, which
    is one clock operation.

    Parameters
    ----------
    df: pandas dataframe
        dataframe with the data to compute. It must have the fields specified
        by the equation along with latency_avg, latency_frequency, occupancy
    equation: dict
        dictionary coming from the JSON file describing additions and
        multiplications. [[], [], []]. Inner lists elems are multiplications,
        outter list elems are additions
    peak_performance: float
        peak performance in MOP/s from the DSP
    '''
    df['Operations'] = 0
    for add in equation:
        df['Multiplier'] = 1
        # Multiply by fields
        for field in add["fields"]:
            df['Multiplier'] *= df[field]
        # Multiply by constants
        for const in add["consts"]:
            df['Multiplier'] *= const
        # Accumulate
        df["Operations"] += df["Multiplier"]

    # Compute resources
    compute_relative_consumption(df)

    # Compute performance
    df['Performance OP/cycle'] = df['Operations'] / df['latency_avg']
    df['Performance MOP/s'] = df['Performance OP/cycle'] * df["latency_frequency"]
    df['Peak Performance MOP/s'] = df['Performance MOP/s'] * df["occupancy"]

    # Compute efficiency w.r.t FPGA
    df["Efficiency w.r.t FPGA"] = df['Peak Performance MOP/s'] / peak_performance

    # Efficiency w.r.t 1 clock cycle solution
    df['Ideal Performance MOP/s'] = df['Operations'] * df["latency_frequency"]
    df['Ideal Peak Performance MOP/s'] = df['Ideal Performance MOP/s'] * df["occupancy"]
    df["Performance Ratio w.r.t Ideal"] = df['Peak Performance MOP/s'] / \
        df['Ideal Peak Performance MOP/s']


def tabulate_model_efficiency(results, opath, effienciency_config, name_template="efficiency.{format}", metrics=["Mean", "Std"], dofs=["Datatype Width", "Datatype Integer", "Core", "PE Columns", "PEs"], formats=["csv", "md"]):
    '''
    Tabulate the model efficiency

    This creates a table to summarise the efficiency results, highlighting: error,
    (mean and std), performance of the accelerator, peak performance of the design,
    and the efficiency (w.r.t to the DSP throughput and w.r.t to the 1 clock
    solution)

    Parameters
    ----------
    df: pandas dataframe
        dataframe with the data to compute. It must have the fields specified
        by the equation along with latency_avg, latency_frequency, occupancy
    opath: string
        Path to save the files
    efficiency_config: dict
        Contains the relevant information such as "equation" and "peak_performance"
        as the dictionary keys
    name_template: string template
        Template for the file name. It should have the "format" template to complete
        with the file extension and "ts" for a timestamp.
    metrics: list of strings
        Contains the error metrics to tabulate
    dofs: list of strings
        Defines the DoFs to tabulate
    formats: list of strings
        Defines the output formats of the tables. Supported: csv, and md

    Returns
    -------
    dataframe: pandas dataframe
        returns the dataframe to be used by other plotters/tabulators because of
        the computation of the efficiency
    '''
    if not len(results):
        print("Nothing to tabulate")
        return

    df_base = results.copy()

    # Compute efficiency and peak performance
    compute_efficiency(
        df_base, effienciency_config["equation"], effienciency_config["peak_performance"])

    # Filter out all the interesting metrics
    efficiency_measures = ["Performance MOP/s", "Peak Performance MOP/s",
                           "Efficiency w.r.t FPGA", "Performance Ratio w.r.t Ideal"]
    df_filtered = df_base[dofs + efficiency_measures +
                          metrics].groupby(dofs, as_index=False).median()

    timestamp = int(time.time())

    # Table construction
    for format in formats:
        if format == 'csv':
            filename = opath + '/' + \
                name_template.format(ts=timestamp, format=format)
            print("Generating Efficiency Table: ", filename, flush=True)
            df_filtered.to_csv(filename, index=False)
        if format == 'md':
            filename = opath + '/' + \
                name_template.format(ts=timestamp, format=format)
            print("Generating Efficiency Table: ", filename, flush=True)
            df_filtered.to_markdown(filename, index=False)
    return df_base
