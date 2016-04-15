#!/bin/bash

mkdir ~/IsletSimBatch/output/SimBatch0001
mkdir ~/IsletSimBatch/output/SimBatch0002

chmod u+x ./slurm0001.sh
chmod o+x ./slurm0002.sh

module load slurm

sbatch slurm0001.sh
sbatch slurm0002.sh