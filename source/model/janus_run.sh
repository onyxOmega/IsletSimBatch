#!/bin/bash

#Setting the name of the job
#PBS -N IsletSimObjectiveTest
#Setting a walltime for the job
#PBS -l walltime=1:30:00
#Selecting processors
#PBS -l nodes=1:ppn=12
#SBATCH --reservation=janus-serial
 
cd $PBS_O_WORKDIR

#Execute

rm runtimeOutput.txt
icpc -std=c++0x -I /projects/fischwil/boost_1_54_0/ -fopenmp -DNUM_CORES=12 main.cpp islet-simulator.cpp islet-file-handler.cpp ch-r2.cpp -o sim_janus.exe
./sim_janus.exe input/UserDefinedVars.txt >> runtimeOutput.txt
