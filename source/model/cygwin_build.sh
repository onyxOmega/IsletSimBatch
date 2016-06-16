#!/bin/bash

g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp -DNUM_CORES=2 main.cpp islet-simulator.cpp islet-file-handler.cpp ch-r2.cpp -o sim_cygwin.exe
./sim_cygwin.exe input/UserDefinedVars.txt
