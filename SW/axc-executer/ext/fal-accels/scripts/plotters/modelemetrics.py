#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import time as time


def tabulate_model_emetrics(results, opath, name_template="emetrics.{format}", metrics=["MSE", "Mean", "Frobenius"], dofs=["Datatype Width", "Datatype Integer", "Core", "PE Columns"], formats=["csv", "md"]):
    '''
    Tabulates the model error metrics. This includes the ones related to the
    model: MSE as the risk function to minimise (model), Mean as the statistic
    to determine the most common error measure (paired events), and Frobenius
    to determine the similarity of two matrices (the data as a whole).

    results: pandas dataframe
        dataframe with the data to compute.
    opath: string
        Path to save the files
    name_template: string template
        Sets the format for the table. It must have the "format" template
        to save the table in the specified format
    metrics: list of strings
        List of error metrix to specify in the table. Usually, mean, mse, and
        frobenius
    dofs: list of strings
        List of DoFs to place also as columns as independent variables
    formats: list of strings
        List of the wanted output formats. Supported: csv, md
    '''
    if not len(results):
        print("Nothing to tabulate")
        return

    df_filtered = results[dofs +
                          metrics].groupby(dofs, as_index=False).median()

    timestamp = int(time.time())

    print("Generating emetrics table", flush=True)

    for format in formats:
        if format == 'csv':
            df_filtered.to_csv(opath + '/' + name_template.format(
                ts=timestamp, format=format), index=False)
        if format == 'md':
            df_filtered.to_markdown(opath + '/' + name_template.format(
                ts=timestamp, format=format), index=False)
