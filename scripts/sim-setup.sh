#!/bin/bash

~/IsletSimBatch/exe/simBatcher.exe

chmod u+x ./directory-setup.sh
chmod o+x ./directory-setup.sh
chmod u+x ./run-batches.sh
chmod o+x ./run-batches.sh

~/IsletSimBatch/scripts/directory-setup.sh
~/IsletSimBatch/scripts/run-batches.sh