#!/bin/bash

g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp ../source/batcher/batch-main.cpp -o ../exe/simBatcher.exe
../exe/simBatcher.exe

chmod u+x ./setupDirectories.sh
chmod o+x ./setupDirectories.sh
chmod u+x ./runBatches.sh
chmod o+x ./runBatches.sh

~/IsletSimBatch/scripts/setupDirectories.sh