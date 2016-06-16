#!/bin/bash

g++ -std=c++0x -I /home/boost_1_54_0/ -fopenmp ../source/batcher/batchMain.cpp -o ../exe/simBatcher.exe
../exe/simBatcher.exe

chmod u+x ./directory-setup.sh
chmod o+x ./directory-setup.sh
chmod u+x ./run-batches.sh
chmod o+x ./run-batches.sh

~/IsletSimBatch/scripts/directory-setup.sh