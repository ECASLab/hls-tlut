#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import re
import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from itertools import product


def plot_consumption(results, opath, xaxis_stickies=["Datatype Width", "Datatype Integer"], name_template="consumption-datatype_{project}.svg", other_stickies=["PE Columns", "Core"]):
    '''
    Generates the plot related to consumption

    It receives the X as stickies: it will generate kinds of plots depending on the stickies.
    Other stickies will generate series within the plots.

    The idea is to generate a plot of one X stickie vs the relative consumptions w.r.t. BRAM,
    FFs, LUTs or DSPs.

    Parameters
    ----------
    results: pandas dataframe
        Dataframe to process
    opath: string
        path to the output files
    xaxis_stickies: list of strings
        X axis data. Each element will lead to different plots
    name_template: string template
        Name of the file as a template. It must have the project variable within the template
    other_stickies: list of strings
        File stickies variables. The stickies will generate new plots
    '''
    if not len(results):
        print("Nothing to tabulate")
        return

    # Resources: they will act as the series
    resources = ["dsp", "lut", "bram", "ff"]

    # Relevant columns
    columns = ["latency_min", "latency_max", "latency_avg",
               "resources_dsp", "resources_lut", "resources_bram", "resources_ff",
               "available_dsp", "available_lut", "available_bram", "available_ff",
               "latency_mintime", "latency_maxtime", "latency_avgtime"]

    # Clean the dataframe by filtering only the required data
    df_filtered = results[xaxis_stickies + columns + other_stickies]

    # Compute relative resources
    for res in resources:
        df_filtered["relative_" + res] = df_filtered["resources_" +
                                                     res] / df_filtered["available_" + res]

    # Iterates over the X Stickies. The idea is to process a plot per stickie
    for dt in xaxis_stickies:
        stickers = {}
        # Filter out the other stickies to avoid processing repetitive data
        stickerlist = list(other_stickies + xaxis_stickies)
        stickerlist.remove(dt)
        for sticker in stickerlist:
            stickers[sticker] = df_filtered[sticker].drop_duplicates()

        # Get permutations of the stickies
        perms = [dict(zip(stickers, v)) for v in product(*stickers.values())]
        for perm in perms:
            # Filter out results: generates a series per permutation of the stickies
            df_filtered_perm = df_filtered.loc[(
                results[list(perm)] == pd.Series(perm)).all(axis=1)]
            df_filtered_perm = df_filtered_perm.groupby(
                dt, as_index=False).mean()

            plt.figure()
            cnt = -1.5
            width = 0.2
            # The range will depend on the available data in the X stickie
            xaxis = np.arange(
                len(df_filtered_perm[dt].drop_duplicates().unique()))

            # Get each series of bars. The series are
            for res in resources:
                columnname = "relative_" + res
                plt.bar(xaxis + (cnt * width),
                        df_filtered_perm[columnname] * 100, width=width, label=res.upper())
                cnt += 1

            # Figure closure
            plt.rcParams.update({'font.size': 14})
            plt.rcParams["font.family"] = "monospace"
            plt.yticks(fontsize=14)
            plt.xticks(fontsize=14)
            plt.legend()
            plt.xticks(xaxis, df_filtered_perm[dt].drop_duplicates().unique())
            plt.xlabel(dt)
            project = re.sub("['\{\}]", '', str(perm)).replace(
                ",", "_").replace(":", "-")
            print("Generating consumption plot", flush=True)
            plt.savefig(opath + "/" +
                        name_template.format(project=project), bbox_inches='tight')
