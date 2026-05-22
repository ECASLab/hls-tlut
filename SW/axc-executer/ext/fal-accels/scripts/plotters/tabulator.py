#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import time


def tabulate_results(results, opath, name_template="summary.{format}", formats=["csv", "md"]):
    '''
    Tabulates the results in a string format for better visualisation

    Parameters
    ----------
    results: pandas dataframe
        dataframe with the data to compute.
    opath: string
        Path to save the files
    name_template: string template
        Sets the format for the table. It must have the "format" template
        to save the table in the specified format
    formats: list of strings
        List of the wanted output formats. Supported: csv, md
    '''
    if not len(results):
        print("Nothing to tabulate")
        return

    timestamp = int(time.time())
    df = results.copy()

    # Get rid of the following columns:
    df.pop("Histogram")
    df.pop("HistMin")
    df.pop("HistMax")
    df.pop("HistBins")
    df.pop("Differences")

    # Affectation table
    for format in formats:
        if format == 'csv':
            filename = opath + '/' + \
                name_template.format(ts=timestamp, format=format)
            print("Generating Efficiency Table: ", filename, flush=True)
            df.to_csv(filename, index=False)
        if format == 'md':
            filename = opath + '/' + \
                name_template.format(ts=timestamp, format=format)
            print("Generating Efficiency Table: ", filename, flush=True)
            df.to_markdown(filename, index=False)
