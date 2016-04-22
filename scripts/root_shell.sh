#!/bin/bash 
mkdir ~/IsletSimBatch/data/SimBatch0002
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001/rep01
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001/rep02
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001/rep03
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001/rep04
chmod u+x slurm0.sh
chmod u+x slurm1.sh
chmod u+x slurm2.sh
chmod u+x slurm3.sh
sbatch slurm0.sh
sbatch slurm1.sh
sbatch slurm2.sh
sbatch slurm3.sh
rm slurm* 
