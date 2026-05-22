#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import matplotlib.pyplot as plt
import pandas as pd
from itertools import product
from textwrap import wrap

def plot_histogram(results, opath, maxrange=1, name_template="hist{project}.png", series_dofs=["Datatype Width"], errors_cols=["Differences"], dofs_filtered=["Datatype Integer", "Core", "PE Columns", "Big Matrix Inputs", "Big Matrix Outputs"]):
    '''
    Plots the error histogram

    Generates several histograms based on DoFs. It will generate the
    permutations of the DoFs passed through "dofs_filtered".

    Parameters
    ----------
    results: pandas dataframe
        Pandas dataframe to get the results from.
    opath: string
        Path to save the results
    maxrange: float
        Sets the maximum error that can be recorded by the bins (maximum bin)
    name_template: string template
        Sets the format for the histograms. It must have the "project" template
        to save the DoF permutation
    series_dofs: list of strings
        Sets the column to be used as series. Most of the time, it is Data Width
    errors_cols: list of strings
        Sets the column to be used as histogram. This function will compute the
        histogram from the differences contained in this column.
    dofs_filtered: list of strings
        Degrees of Freedom used for permutations and setting the configurations
        of the accelerator while generating the histograms
    '''
    if not len(results):
        print("Nothing to tabulate")
        return

    # Filter data by dofs
    dofs = {}
    for dof in dofs_filtered:
        dofs[dof] = results[dof].drop_duplicates().unique()

    # Get permutations
    perms = [dict(zip(dofs, v)) for v in product(*dofs.values())]

    # Start plotting. One plot will be generated from the permutation of
    # the DoFs passed to the function as dofs_filtered
    for perm in perms:
        # Filter out results according to DOFs
        df_filtered = results.loc[(
            results[list(perm)] == pd.Series(perm)).all(axis=1)]
        df_filtered_columns = series_dofs + errors_cols + dofs_filtered
        df_filtered = df_filtered[df_filtered_columns]

        # Filter out the series
        series = {}
        for serie in series_dofs:
            series[serie] = df_filtered[serie].drop_duplicates().unique()

        permseries = [dict(zip(series, v)) for v in product(*series.values())]

        # Set all configurations for the picture
        plt.figure()
        title = ""
        name = ""
        for dof in perm.keys():
            title += dof + ": "
            title += str(perm[dof]) + " "
            name += "_" + dof + "-" + str(perm[dof])
        name = name.replace(' ', '')
        figname = opath + '/' + name_template.format(project=name)

        # Plot series
        for serie in permseries:
            reduction = {}
            reduction[errors_cols[0]] = 'sum'
            df_filtered_series = df_filtered.loc[(
                results[list(serie)] == pd.Series(serie)).all(axis=1)]
            df_filtered_series = df_filtered_series[errors_cols]
            histogram = list(df_filtered_series.agg(reduction))
            n, x, patches = plt.hist(histogram, alpha=0.6, zorder=1/float(serie[series_dofs[0]]), antialiased=True,
                     density=True, stacked=True, histtype="stepfilled", linewidth=2, bins=20, range=(0, maxrange),
                     label=serie)
            bin_centers = 0.5*(x[1:]+x[:-1])
            [p.remove() for p in patches]
            plt.plot(bin_centers, n, label=serie, alpha=0.7, zorder=1/float(serie[series_dofs[0]])) ## using bin_centers rather than edges

        plt.rcParams.update({'font.size': 14})
        plt.rcParams["font.family"] = "monospace"
        plt.xticks(fontsize=14)
        plt.xscale("log")

        plt.title("\n".join(wrap(title,35)) + "\n", wrap=True)

        plt.grid(True)
        plt.legend()
        print("Generating error histogram plot: ", figname, flush=True)
        plt.yticks([], [])
        plt.savefig(figname, bbox_inches='tight')
        plt.xscale("linear")
