#!/bin/bash 
#Setting the name of the job 
#PBS -N IsletSimObjectiveTest 
#Setting a walltime for the job 
#PBS -l walltime=00:17:50
#Selecting processors 
#PBS -l nodes=1:ppn=12 
#SBATCH --reservation=janus-serial 
#SBATCH --output=../data/SimBatch0005/sim0003/slurm.out 
cd $PBS_O_WORKDIR 

#Execute 

~/IsletSimBatch/exe/simulator.exe Batch:5 Sim:3