#!/bin/bash

#Setting the name of the job
#PBS -N IsletSimObjectiveTest
#Setting a walltime for the job
#PBS -l walltime=0:05:00
#Selecting processors
#PBS -l nodes=1:ppn=12
#SBATCH --reservation=janus-serial
#SBATCH --output=../output/SimBatch0002/out.txt
 
cd $PBS_O_WORKDIR

#Execute

~/IsletSimBatch/exe/simulator.exe ../output/SimBatch0002/