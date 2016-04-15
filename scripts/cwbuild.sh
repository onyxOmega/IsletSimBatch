#!/bin/bash

g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp ../source/model/simulator.cpp -o ../exe/simulator.exe
g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp ../source/batcher/simBatcher.cpp -o ../exe/simBatcher.exe