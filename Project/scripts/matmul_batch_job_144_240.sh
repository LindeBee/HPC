#!/bin/bash -l

# The name of the script is matmul_batch_job
#SBATCH -J matmulBatchRun144_240
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 00:05:00
#SBATCH -A edu20.DD2356
# Number of nodes
#SBATCH --nodes=5
#SBATCH --ntasks-per-node=32
#SBATCH -C Haswell
#SBATCH -e error_file144_240.e

# Run the executable file
# and write the output into my_output_file
srun -n 144 ./mat_mul_opt_240 > ./results/out144_240
