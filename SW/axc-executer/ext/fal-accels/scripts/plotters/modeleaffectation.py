#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import time as time
import matplotlib.pyplot as plt
import numpy as np


def tabulate_model_eaffectation(results, opath, name_template="eaffectation.{format}", metrics=["Mean", "Min", "Max", "Std"], dofs=["Datatype Width", "Datatype Integer", "Core", "PE Columns"], formats=["csv", "md"]):
    '''
    Tabulates the model error affectation with respect to the configurations
    of the synthesised design

    This creates a table summarising the error metrics with respect to the
    degrees of freedom of the design

    Parameters
    ----------
    results: pandas dataframe
        dataframe with the data to compute.
    opath: string
        Path to save the files
    name_template: string template
        Sets the format for the table. It must have the "format" template
        to save the table in the specified format
    metrics: list of strings
        List of error metrix to specify in the table. Usually, mean, min, max,
        and variance are placed as elements
    dofs: list of strings
        List of DoFs to place also as columns as independent variables
    formats: list of strings
        List of the wanted output formats. Supported: csv, md
    '''
    if not len(results):
        print("Nothing to tabulate")
        return

    # Filter out the elements not specified in either dofs or metrics
    # Compute the median to get the majoritary element.
    df_filtered = results[dofs +
                          metrics].groupby(dofs, as_index=False).median()

    timestamp = int(time.time())

    # Affectation table
    for format in formats:
        if format == 'csv':
            filename = opath + '/' + \
                name_template.format(ts=timestamp, format=format)
            print("Generating Error Affectation Table: ", filename, flush=True)
            df_filtered.to_csv(filename, index=False)
        if format == 'md':
            filename = opath + '/' + \
                name_template.format(ts=timestamp, format=format)
            print("Generating Error Affectation Table: ", filename, flush=True)
            df_filtered.to_markdown(filename, index=False)


def plot_model_eaffectation(results, opath, name_template="eaffectation.pdf", differences=['Differences'], dofs=["Datatype Width", "Datatype Integer", "Core", "PE Columns"]):
    '''
    Plots the model error affectation with respect to the configurations
    of the synthesised design. It is similar to @plot_model_eaffectation.

    The output plot is a box plot with the DoFs permutations as X ticks.

    Parameters
    ----------
    results: pandas dataframe
        dataframe with the data to compute.
    opath: string
        Path to save the files
    name_template: string template
        Sets the format for the plot. It can have the "ts" template for placing
        a timestamp
    differences: list of strings
        List of differences to compute the box plot
    dofs: list of strings
        List of DoFs to place also as columns as independent variables
    '''

    if not len(results):
        print("Nothing to tabulate")
        return

    # Filter data by dofs
    df_filtered_max = results[results["Max"] < 2]
    df_filtered = df_filtered_max[dofs + differences]
    df_filtered = df_filtered.groupby(dofs, as_index=False).sum()
    df_filtered["Config"] = df_filtered[dofs].apply(
        lambda row: ' '.join(row.values.astype(str)), axis=1)
    df_filtered = df_filtered[["Config"] + differences]
    df_dict = df_filtered.to_dict(orient="list")

    plt.figure()
    plt.boxplot(df_dict[differences[0]], showfliers=False,
                vert=True, labels=df_dict["Config"])
    plt.xticks(rotation=90)
    plt.grid()
    plt.tight_layout()

    timestamp = int(time.time())
    figname = opath + '/' + name_template.format(ts=timestamp)
    print("Generating Error Affectation Plot: ", figname, flush=True)
    plt.savefig(figname)
