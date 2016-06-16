#!/bin/bash

# Use this script to compile and run a simulation in Janus. It loads the appropriate modules, compiles with the right number of cores, and so on. Meant to minimize the number of steps required to start a simulation.

module load slurm
module load intel
sbatch janus_run.sh