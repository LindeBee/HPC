#!/bin/bash -l

# The name of the script is matmul_batch_job
#SBATCH -J matmulBatchRun36_240
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 00:05:00
#SBATCH -A edu20.DD2356
# Number of nodes
#SBATCH --nodes=2
#SBATCH --ntasks-per-node=32
#SBATCH -C Haswell
#SBATCH -e error_file36_240.e

# Run the executable file
# and write the output into my_output_file
srun -n 36 ./mat_mul_opt_240 > ./results/out36_240
