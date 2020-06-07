#!/bin/bash -l

# The name of the script is matmul_batch_job
#SBATCH -J matmulBatchRun1_2048
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 00:05:00
#SBATCH -A edu20.DD2356
# Number of nodes
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=32
#SBATCH -C Haswell
#SBATCH -e error_file1_2048.e

# Run the executable file
# and write the output into my_output_file
srun -n 1 ./mat_mul_opt_2048 > ./results/out1_2048
