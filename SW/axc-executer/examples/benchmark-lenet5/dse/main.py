#!/usr/bin/env python3
############################################################
# Copyright 2023
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

from mpi4py import MPI
import numpy as np
import os
import pickle

from dse.space import *
from dse.utils import *
from dse.launcher import *
from dse.template import *

comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size = comm.Get_size()

dse_result_path = "dse.results.log"

def scatter(jobs):
    '''
    Scatters the data amongst the jobs
    '''
    chunks = None
    if jobs:
        chunks = np.split(np.array(jobs, dtype=object), size)
    data = comm.scatter(chunks, root=0)
    return data

def gather(data):
    '''
    Gathers the data amongst the jobs to the master
    '''
    data = comm.gather(data, root=0)
    if data is not None:
        data = np.concatenate(data, axis=None)

    return data

def launch_job(jobs, idx):
    all_env = {**os.environ}
    job = jobs[idx]
    njobs = len(jobs)
    job_result = {'conv': job[0], 'dense': job[1], 'accuracy': -1}

    # Create environment
    cmd = f"bash scripts/create-environment.sh {rank}"
    data = run_process(cmd, all_env)
    
    if data['code'] != 0:
        print(f"Error while executing job {idx}/{njobs}: {job}")
        return job_result

    print(f"Worker {rank} created the folder successfully {idx}/{njobs}", flush=True)
    iterdir = data['stdout']

    # Fill template
    res = fill_template("templates/config.hpp.in", job)
    f = open(f"{iterdir}/examples/mqlenet5/config.hpp", "w")
    f.write(res)
    f.close()

    # Start compilation
    cmd = f"bash scripts/start-compilation.sh {iterdir}"
    data = run_process(cmd, all_env)
    
    if data['code'] != 0:
        print(f"Error while compiling job {idx}/{njobs}: {data['stdout']}, {job}")
        return job_result

    print(f"Worker {rank} compiled the job successfully {idx}/{njobs}", flush=True)

    # Run job
    cmd = f"bash scripts/run-job.sh {iterdir}"
    data = run_process(cmd, all_env)
    
    if data['code'] != 0:
        print(f"Error while executing job {idx}/{njobs}: {data['stdout']}, {job}")
        return job_result

    print(f"Worker {rank} runned the job successfully {idx}/{njobs} with acc {data['stdout']}", flush=True)
    job_result['accuracy'] = data['stdout']
    return job_result

def save(part):
    '''
    Saves progress
    '''
    with open(f"{dse_result_path}-{rank}", 'a') as f:
        f.write(str(part) + '\n')

def master():
    '''
    Performs the master jobs
    '''
    global dse_result_path

    all_jobs = []
    jobs_path = os.environ.get('JOBS_PATH', None)
    if jobs_path is None:
        all_jobs, conv, dense = get_permutations()
        # Dump the permutations
        with open('jobs.pkl', 'wb') as f:
            pickle.dump(all_jobs, f)
    else:
        # Load permutations
        with open(jobs_path, 'rb') as f:
            all_jobs = pickle.load(f)
    
    # Select jobs
    start_idx = os.environ.get('START_PERMUTATION', 0)
    end_idx = os.environ.get('END_PERMUTATION', int(1e9))
    jobs = all_jobs[int(start_idx):int(end_idx)]
    print(f"Selecting permutations from {start_idx} to {end_idx}. Total {len(jobs)}/{len(all_jobs)}")

    print(f"Master will scatter {len(jobs)}", flush=True)
    # Scatter and perform jobs
    myjobs = scatter(jobs)
    print(f"Worker {rank} has {len(myjobs)} in charge", flush=True)
    
    # Run job and save point
    res = []
    for i in range(len(myjobs)):
        part = launch_job(myjobs, i)
        res.append(part)
        save(part)

    # Gather results
    gathered = gather(res)
    save(gathered)


def child():
    '''
    Performs the children jobs
    '''
    # Get work
    myjobs = scatter(None)
    print(f"Worker {rank} has {len(myjobs)} in charge", flush=True)

    # Run job and save point
    res = []
    for i in range(len(myjobs)):
        part = launch_job(myjobs, i)
        res.append(part)
        save(part)

    # Gather results
    gather(res)
    print(f"Worker {rank} has finished", flush=True)

if __name__ == "__main__":
    if rank == 0:
        print_header()
        master()
    else:
        child()