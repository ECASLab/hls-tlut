#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import matplotlib.pyplot as plt
from plotters.efficiency import compute_relative_consumption


def initials(s):
    '''
    Gets the initials of the words

    Parameters
    ----------
    s: string
        input string

    Returns
    -------
    initials: string
        output string with the capitalised initials
    '''
    # split the string into a list
    l = s.split()
    new = ""
    # traverse in the list
    for i in range(len(l)):
        s = l[i]
        # adds the capital first character
        new += (s[0].upper())
    return new


def plot_trade_off(df, opath, name_template="tradeoff{project}.png", error=["Mean"], latency=["latency_avg"], core=["Core"], dofs=["PEs", "PE Columns", "Datatype Width", "Datatype Integer"], formats=["csv", "md"]):
    '''
    Generates the trade-off plot.

    The trade-off plot summarises the Latency vs the Area consumption. It is
    widely used in the industry and academia to balance the trade-off between
    having less energy consumption (less area) and more throughput (less
    latency). The plot is a scatter bubble plot, where the size of the bubbles
    contemplates the error

    Parameters
    ----------
    df: pandas dataframe
        dataframe with the data to compute.
    opath: string
        Path to save the files
    name_template: string template
        Sets the format for the table. It must have the "project" template
        to save the plot configuration
    error: list of strings
        List of one element of the column used as error (size of the bubbles)
    latency: list of strings
        List of one element of the column used as latency (X Axis)
    core: list of strings
        List of one element of the core. It will be used as plot generator variable
    dofs: list of strings
        List of columns to be used as degrees of freedom. The permutations will lead
        to the series of bubbles.
    formats: list of strings
        List of the wanted output formats. Supported: csv, md
    '''
    # Copy and get the relative consumption
    df_base = df.copy()
    compute_relative_consumption(df_base)

    # Filter the dataframe by the variables specified as error, latency, core,
    # and dofs. It also includes the "consumption" computed before
    df_filtered = df_base[error + latency +
                          ["consumption"] + dofs + core]
    df_filtered["DoFs"] = initials(dofs[0]) + df_filtered[dofs[0]].astype(str)
    for i in range(1, len(dofs)):
        df_filtered["DoFs"] = df_filtered["DoFs"] + \
            "," + initials(dofs[i]) + df[dofs[i]].astype(str)
    cores = df_base[core].drop_duplicates().values.flatten()
    df_filtered = df_filtered[["DoFs", "consumption"] +
                              error + latency].groupby("DoFs", as_index=False).median()
    df_filtered[core] = df_base[core]
    df_filtered = df_filtered[df_filtered["consumption"] <= 1.]

    # Plot per core: each core will be plotted in a different trade-off plot
    for i in cores:
        plt.figure(figsize=(8, 4), dpi=300)

        df = df_filtered.loc[df_filtered["Core"] == i]
        dofsn = df["DoFs"].values.flatten()

        # Generates a series per DoF permutation
        for j in range(len(dofsn)):
            # X: latency, Y: consumption, Z: bubble size (error), N: Dofs
            x = df[latency].values.flatten()[j]
            y = df["consumption"].values.flatten()[j]
            z = df[error].values.flatten()[j]
            if y > 0.5:
                continue
            if z > 0.5:
                continue
            n = f'{dofsn[j]} E:{(z * 100):.1f} L:{int(x)} C:{y:.2f}'
            plt.scatter(x, y, alpha=0.5, s=z * 1000, label=n)

        plt.title(f"Core:{i}")
        plt.xlabel("Latency in clocks")
        plt.ylabel("Relative Consumption")
        plt.legend(title='Configuration', prop={
                   'size': 8}, loc='center left', bbox_to_anchor=(1.01, 0.5))
        plt.tight_layout()
        print("Generating Trade-off Plot", flush=True)
        plt.savefig(opath + "/" + name_template.format(project=i) +
                    '.pdf', bbox_inches='tight')

        # Save tables
        for format in formats:
            if format == 'csv':
                filename = opath + '/' + \
                    name_template.format(project=i, format=format)
                print("Generating Trade-off Table: ", filename, flush=True)
                df_filtered.to_csv(filename, index=False)
            if format == 'md':
                filename = opath + '/' + \
                    name_template.format(project=i, format=format)
                print("Generating Trade-off Table: ", filename, flush=True)
                df_filtered.to_markdown(filename, index=False)
