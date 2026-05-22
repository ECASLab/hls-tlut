#!/usr/bin/env python3
############################################################
# Copyright 2022
# Author: Luis G. Leon-Vega <luis.leon@ieee.org>
############################################################

import shlex
import subprocess
import sys


def run_process(command, env):
    '''
    Run a process synchronously

    Parameters
    ----------
    command: string
        command to launch in a single string
    env: dictionary
        dictionary with the environment variables to set in subprocess

    Returns
    -------
    result: dictionary
        The dictionary contains "stdout" and "code" as keys, where "stdout" is
        the combination of "stdout" and "stderr". "code" is the return code of
        the program.
    '''

    args = shlex.split(command)

    # Convert to string
    for k in env.keys():
        env[k] = str(env[k])

    res = subprocess.run(
        args, env=env, stderr=subprocess.STDOUT, stdout=subprocess.PIPE)
    return {"stdout": str(res.stdout, "utf_8"), "code": res.returncode}


if __name__ == "__main__":
    print("Runing:", sys.argv[1])
    res = run_process(sys.argv[1], {})
    print(res)
