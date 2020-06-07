#!/bin/bash -l

# The name of the script is matmul_batch_job
#SBATCH -J matmulBatchRun256_2048_vec
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 00:05:00
#SBATCH -A edu20.DD2356
# Number of nodes
#SBATCH --nodes=8
#SBATCH --ntasks-per-node=32
#SBATCH -C Haswell
#SBATCH -e error_file256_2048_vec.e

# Run the executable file
# and write the output into my_output_file
srun -n 256 ./mat_mul_2048_vec > ./results/out256_2048_vec
