#!/bin/bash -l

# The name of the script is matmul_batch_job
#SBATCH -J matmulBatchRun100_240_vec
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 00:05:00
#SBATCH -A edu20.DD2356
# Number of nodes
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=32
#SBATCH -C Haswell
#SBATCH -e error_file100_240_vec.e

# Run the executable file
# and write the output into my_output_file
srun -n 100 ./mat_mul_240_vec > ./results/out100_240_vec
