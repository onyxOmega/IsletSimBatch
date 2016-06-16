#!/bin/bash

g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp -DNUM_CORES=2 ../source/model/main.cpp ../source/model/islet-simulator.cpp ../source/model/islet-file-handler.cpp ../source/model/ch-r2.cpp -o ../exe/cwSimulator.exe
../exe/cwSimulator.exe Batch:2,Sim:1,