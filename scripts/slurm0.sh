#!/bin/bash 
#Setting the name of the job 
#PBS -N IsletSimObjectiveTest 
#Setting a walltime for the job 
#PBS -l walltime=00:17:50
#Selecting processors 
#PBS -l nodes=1:ppn=12 
#SBATCH --reservation=janus-serial 
#SBATCH --output=../data/SimBatch0009/sim0001/slurm.out 
cd $PBS_O_WORKDIR 

#Execute 

../exe/janSim.exe Batch:9,Sim:1,Rep:0