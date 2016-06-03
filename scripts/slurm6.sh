#!/bin/bash 
#Setting the name of the job 
#PBS -N IsletSimObjectiveTest 
#Setting a walltime for the job 
#PBS -l walltime=
#Selecting processors 
#PBS -l nodes=1:ppn=12 
#SBATCH --reservation=janus-serial 
#SBATCH --output=../data/SimBatch0003/sim0001/slurmRep01.out 
cd $PBS_O_WORKDIR 

#Execute 

~/IsletSimBatch/exe/simulator.exe Batch:3 Sim:1 Rep:1