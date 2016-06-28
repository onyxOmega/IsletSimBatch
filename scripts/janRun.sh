#!/bin/bash

../exe/janBatch.exe

chmod u+x ./setupDirectories.sh
chmod o+x ./setupDirectories.sh
chmod u+x ./runBatches.sh
chmod o+x ./runBatches.sh

./setupDirectories.sh
./runBatches.sh