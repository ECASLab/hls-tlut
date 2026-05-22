#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
#############################################################

import argparse
import numpy as np
import pandas as pd
import os

from mpi4py import MPI

from runner.launcher import run_process
from runner.configreader import get_config, get_permutations
from parsers.extractresults import extract_results
from parsers.extractdofs import extract_dofs
from parsers.extractconsumption import extract_consumption
from parsers.extracterrors import extract_errors
from plotters.tabulator import tabulate_results

from plotters.histogram import plot_histogram
from plotters.quality import plot_quality
from plotters.modelemetrics import tabulate_model_emetrics
from plotters.modeleaffectation import tabulate_model_eaffectation, plot_model_eaffectation
from plotters.consumption import plot_consumption
from plotters.efficiency import tabulate_model_efficiency
from plotters.tradeoff import plot_trade_off

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()


def get_files(env):
    '''
    Gets the paths to the files to analyse
    '''
    ks = env.get("Q_KS", 3)
    accel = env["ACCELERATOR"]
    qint = env["Q_INT"]
    qbw = env["Q_BW"]
    qo = env["Q_O"]
    core = env["Q_CORE"]
    pes = env["Q_PES"]
    seed = env["Q_SEED"]
    inputs = env["Q_INPUTS_TB"]
    outputs = env["Q_OUTPUTS_TB"]

    project = f"hls_project_{accel}_{ks}_{qbw}_{qint}_{qo}_{core}_{pes}_{inputs}_{outputs}_{seed}"
    results_file_name = os.listdir(os.path.abspath(
        f"examples/{accel}/{project}/solution/csim/report"))[0]
    results_file = os.path.abspath(
        f"examples/{accel}/{project}/solution/csim/report/" + results_file_name)
    report_file = os.path.abspath(
        f"examples/{accel}/{project}/solution/syn/report/csynth.xml")
    errors_file = os.path.abspath(
        f"examples/{accel}/{project}/solution/csim/build/error-results.log")
    return results_file, report_file, errors_file


def run(env, dse=True, run_sim=True):
    '''
    Launches the jobs for collecting data
    '''
    # Prepare command
    env["Q_PID"] = rank

    # Fix the TB ARGV
    if env.get("TB_ARGV", None):
        filename = os.path.splitext(os.path.basename(env["TB_ARGV"]))[0]
        env["Q_SEED"] = filename

    print(env, flush=True)
    all_env = {**env, **os.environ}
    syn_cmd = f'make synthesis'
    data = None

    if run_sim:
        # Run the synthesis
        data = run_process(syn_cmd, all_env)
        print("Rank", rank, env)
        if data["code"]:
            # Clean the undesired project
            print(f"Command error in rank {rank}:", data["stdout"])
            return None

    # Postprocess data
    if dse:
        files = get_files(env)
        data = {}
        try:
            data = {
                **extract_results(files[0]),
                **extract_dofs(files[0]),
                **extract_consumption(files[1]),
                **extract_errors(files[2])
            }
        except FileNotFoundError as e:
            print("Error:", e)
            data = None

    return data


def gather(data):
    '''
    Gathers the data amongst the jobs to the master
    '''
    data = comm.gather(data, root=0)
    if data is not None:
        data = np.concatenate(data, axis=None)

    if rank == 0:
        # With all results, everything is a list
        df = pd.DataFrame.from_records(data)
        return df


def scatter(jobs):
    '''
    Scatters the data amongst the jobs
    '''
    chunks = None
    if jobs:
        chunks = np.split(np.array(jobs), size)
    data = comm.scatter(chunks, root=0)
    print(f"Rank {rank} data:", len(data), flush=True)
    return data


def master(file, opath, dse=True, run_sim=True):
    '''
    Runs the master process. This is intended to configure jobs
    and collect the results for later processing
    '''
    # Get the jobs
    config = get_config(file)

    consumption_pairs = config.get("CONSUMPTION_PAIRS", [])
    config.pop("CONSUMPTION_PAIRS")
    quality_pairs = config.get("QUALITY_PAIRS", [])
    config.pop("QUALITY_PAIRS")
    histogram_config = config.get("HISTOGRAM", {})
    config.pop("HISTOGRAM")
    effienciency_config = config.get("EFFICIENCY", {})
    config.pop("EFFICIENCY")
    error_config = config.get("ERROR", {})
    config.pop("ERROR")
    tradeoff = config.get("TRADE-OFF", {})
    config.pop("TRADE-OFF")

    jobs = get_permutations(config)
    print(f"Master will distribute {len(jobs)} jobs\n", flush=True)
    myjobs = scatter(jobs)
    # Run results and submit them for the gathering
    results = []
    for j in myjobs:
        datum = run(j, dse, run_sim)
        if datum is not None:
            results.append(datum)
    # Post-process
    if dse:
        gathered = gather(results)
        print(f"Gathered: {len(gathered)} results", flush=True)
        plot_histogram(gathered, opath, name_template=histogram_config["template"],
                       dofs_filtered=histogram_config["stickies"], maxrange=histogram_config["maxrange"])
        for pair in quality_pairs:
            plot_quality(gathered, opath, quality_dofs=pair["dof"], name_template=pair["template"],
                         series_dofs=pair["series"], dofs_filtered=pair["stickies"])

        tabulate_model_emetrics(gathered, opath, dofs=error_config["dofs"])
        tabulate_model_eaffectation(gathered, opath, dofs=error_config["dofs"])
        plot_model_eaffectation(gathered, opath, dofs=error_config["dofs"])
        df_w_efficiency = tabulate_model_efficiency(
            gathered, opath, effienciency_config=effienciency_config)
        tabulate_results(df_w_efficiency, opath)
        plot_trade_off(gathered, opath, error=tradeoff["error"], latency=tradeoff["latency"],
                       dofs=tradeoff["dofs"], core=tradeoff["core"], name_template=tradeoff["template"])

        for pair in consumption_pairs:
            plot_consumption(gathered, opath, xaxis_stickies=pair["x"],
                             other_stickies=pair["stickies"], name_template=pair["template"])


def child(dse=True, run_sim=True):
    '''
    Runs the client processes. This is intended for running jobs
    and submit the results to the masters: All-to-One
    '''
    # Get jobs
    myjobs = scatter(None)
    # Run results and submit them for the gathering
    results = []
    for j in myjobs:
        datum = run(j, dse, run_sim)
        if datum is not None:
            results.append(datum)
    if dse:
        gather(results)


def argument_handler():
    parser = argparse.ArgumentParser(
        description='DSE tool for Flexible Accelerators Library (FAL)')
    parser.add_argument('configfile', metavar='FILE', type=str, nargs=1,
                        help='Configuration file for the run. For instance: ./scripts/configs/gemm.json')
    parser.add_argument('--no-run', type=bool, default=False,
                        help='Do not run the simulation process to get results')
    parser.add_argument('--no-dse', type=bool, default=False,
                        help='Do not run the DSE analysis')
    parser.add_argument('--output-path', type=str, default='results',
                        help='Output path of the results')
    args = parser.parse_args()
    return args


def header():
    print('''
-------------------------------------------------------------------------------
-- Flexible Accelerators Library (FAL)                                       --
-- ECASLab 2021-2022                                                         --
-------------------------------------------------------------------------------'''
          , flush=True)


if __name__ == "__main__":
    args = argument_handler()
    file = args.configfile[0]
    run_sim = not args.no_run
    dse = not args.no_dse
    opath = args.output_path
    if rank == 0:
        header()
        print("DSE Config File:", file, flush=True)
        print(f"Performing run: {run_sim}, dse: {dse}\n", flush=True)
        print(f"Running in parallel with: {size} processes", flush=True)
        master(file, opath, dse, run_sim)
    else:
        child(dse, run_sim)
