#!/bin/bash
module load intel
icpc -std=c++0x -I /projects/fischwil/boost_1_54_0/ -fopenmp -DNUM_CORES=12 main.cpp islet-simulator.cpp islet-file-handler.cpp ch-r2.cpp -o sim_janus.exe