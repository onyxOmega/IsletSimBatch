#!/bin/bash

g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp -DNUM_CORES=2 ../source/model/islet-main.cpp ../source/model/islet-simulator.cpp ../source/model/islet-file-handler.cpp  -o ../exe/cwSimulator.exe
../exe/cwSimulator.exe Batch:99,Sim:99,Rep:99