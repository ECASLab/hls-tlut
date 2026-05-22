#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import matplotlib.pyplot as plt
import pandas as pd
import itertools
from textwrap import wrap

def plot_quality(results, opath, name_template="PSNR{project}.png", series_dofs=["Core", "PE Columns", "PEs"], quality_dofs=["PSNR"], horizontal_dof=['Datatype Width'], dofs_filtered=["Datatype Integer", "Big Matrix Inputs", "Big Matrix Outputs"]):
    '''
    Generates a quality plot. The quality of the accelerators are referred to
    the quality of the signal over the noise (quantisation + operator error).
    The quality is often measured by the PSNR and SSIM.

    Parameters
    ----------
    results: pandas dataframe
        dataframe with the data to compute.
    opath: string
        Path to save the files
    name_template: string template
        Sets the format for the table. It must have the "project" template
        to save the plot according to the configuration. The project template
        will be filled with the "dofs_filtered" list
    series_dofs: list of strings
        List with the DoFs to be listed as series within the plot.
    quality_dofs: list of strings
        List with one element summarising the quality metric. It can be: PSNR
        and SSIM. However, it is not limited to them
    horizontal_dof: list of strings
        List with one element to collect the X Axis data.
    dofs_filtered: list of strings
        List of degrees of freedom that will lead to permutations and several
        plots.
    '''
    if not len(results):
        print("Nothing to tabulate")
        return

    # Filter data by dofs
    dofs = {}
    for dof in dofs_filtered:
        dofs[dof] = results[dof].drop_duplicates().unique()

    # Get permutations
    perms = [dict(zip(dofs, v)) for v in itertools.product(*dofs.values())]

    # Start plotting. Generates a plot per permutation of the dofs
    for perm in perms:
        # Filter out results according to DOFs
        df_filtered = results.loc[(
            results[list(perm)] == pd.Series(perm)).all(axis=1)]
        df_filtered = df_filtered[series_dofs +
                                  quality_dofs + horizontal_dof].drop_duplicates()
        df_filtered = df_filtered.set_index(horizontal_dof).sort_index()

        # Filter out the series
        series = {}
        for serie in series_dofs:
            series[serie] = df_filtered[serie].drop_duplicates().unique()

        permseries = [dict(zip(series, v)) for v in itertools.product(*series.values())]

        # Set all configurations for the picture
        marker = itertools.cycle(('+', 'o', '*', 'x')) 
        plt.figure()
        title = ""
        name = ""
        for dof in perm.keys():
            if dof == "Image name":
                continue
            title += dof + ": "
            title += str(perm[dof]) + " "
            name += "_" + dof + "-" + str(perm[dof])
        name = name.replace(' ', '')
        name = name.replace('/', '-')
        name = name.replace('.', '-')
        figname = opath + '/' + name_template.format(project=name)
        plt.rcParams.update({'font.size': 14})
        plt.rcParams["font.family"] = "monospace"
        plt.yticks(fontsize=14)
        plt.xticks(fontsize=14)

        plt.title("\n".join(wrap(title,35)) + "\n", wrap=True)
        plt.grid(True)

        # Plot series
        for serie in permseries:
            df_filtered_series = df_filtered.loc[(
                df_filtered[list(serie)] == pd.Series(serie)).all(axis=1)]
            df_filtered_series = df_filtered_series[quality_dofs].drop_duplicates(
            )
            plt.plot(df_filtered_series, marker=next(marker), alpha=0.7,
                     antialiased=True, linewidth=1, label=str(serie))

        print("Generating Quality Plot: ", figname, flush=True)
        plt.savefig(figname, bbox_inches='tight')
