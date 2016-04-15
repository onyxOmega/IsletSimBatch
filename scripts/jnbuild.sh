#!/bin/bash

module load intel
icpc -std=c++0x -I ~/boost_1_54_0/ -fopenmp ~/IsletSimBatch/source/model/simulator.cpp -o  ~/IsletSimBatch/exe/simulator.exe
icpc -std=c++0x -I ~/boost_1_54_0/ -fopenmp ~/IsletSimBatch/source/batcher/simBatcher.cpp -o  ~/IsletSimBatch/exe/simBatcher.exe