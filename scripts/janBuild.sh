#!/bin/bash

module load intel
icpc -std=c++0x -I /projects/fischwil/boost_1_54_0/ -fopenmp -DNUM_CORES=12 ../source/model/islet-main.cpp ../source/model/islet-simulator.cpp ../source/model/islet-file-handler.cpp -o ../exe/janSim.exe
icpc -std=c++0x -I /projects/fischwil/boost_1_54_0/ -fopenmp ../source/batcher/batch-main.cpp -o ../exe/janBatch.exe