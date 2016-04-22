#!/bin/bash

g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp ../source/model/modelMain.cpp -o ../exe/simulator.exe
../exe/simulator.exe ../output/SimBatch0001/

head ../data/SimBatch0001/testOut.txt