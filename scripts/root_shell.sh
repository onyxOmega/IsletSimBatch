#!/bin/bash 
mkdir ~/IsletSimBatch/data/SimBatch0002
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001/rep01
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001/rep02
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0001/rep03
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0002
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0002/rep01
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0002/rep02
mkdir ~/IsletSimBatch/data/SimBatch0002/sim0002/rep03
mkdir ~/IsletSimBatch/data/SimBatch0003
mkdir ~/IsletSimBatch/data/SimBatch0003/sim0001
mkdir ~/IsletSimBatch/data/SimBatch0003/sim0001/rep01
mkdir ~/IsletSimBatch/data/SimBatch0003/sim0001/rep02
mkdir ~/IsletSimBatch/data/SimBatch0003/sim0002
mkdir ~/IsletSimBatch/data/SimBatch0003/sim0002/rep01
mkdir ~/IsletSimBatch/data/SimBatch0003/sim0002/rep02
mkdir ~/IsletSimBatch/data/SimBatch0004
mkdir ~/IsletSimBatch/data/SimBatch0004/sim0001
mkdir ~/IsletSimBatch/data/SimBatch0004/sim0002
mkdir ~/IsletSimBatch/data/SimBatch0005
mkdir ~/IsletSimBatch/data/SimBatch0005/sim0001
mkdir ~/IsletSimBatch/data/SimBatch0005/sim0002
mkdir ~/IsletSimBatch/data/SimBatch0005/sim0003
mkdir ~/IsletSimBatch/data/SimBatch0005/sim0004
mkdir ~/IsletSimBatch/data/SimBatch0005/sim0005
mkdir ~/IsletSimBatch/data/SimBatch0005/sim0006
chmod u+x slurm0.sh
chmod u+x slurm1.sh
chmod u+x slurm2.sh
chmod u+x slurm3.sh
chmod u+x slurm4.sh
chmod u+x slurm5.sh
chmod u+x slurm6.sh
chmod u+x slurm7.sh
chmod u+x slurm8.sh
chmod u+x slurm9.sh
chmod u+x slurm10.sh
chmod u+x slurm11.sh
chmod u+x slurm12.sh
chmod u+x slurm13.sh
chmod u+x slurm14.sh
chmod u+x slurm15.sh
chmod u+x slurm16.sh
chmod u+x slurm17.sh
sbatch slurm0.sh
sbatch slurm1.sh
sbatch slurm2.sh
sbatch slurm3.sh
sbatch slurm4.sh
sbatch slurm5.sh
sbatch slurm6.sh
sbatch slurm7.sh
sbatch slurm8.sh
sbatch slurm9.sh
sbatch slurm10.sh
sbatch slurm11.sh
sbatch slurm12.sh
sbatch slurm13.sh
sbatch slurm14.sh
sbatch slurm15.sh
sbatch slurm16.sh
sbatch slurm17.sh
rm slurm* 
