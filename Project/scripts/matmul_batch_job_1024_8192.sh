#!/bin/bash -l

# The name of the script is matmul_batch_job
#SBATCH -J matmulBatchRun1024_8192
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 00:05:00
#SBATCH -A edu20.DD2356
# Number of nodes
#SBATCH --nodes=32
#SBATCH --ntasks-per-node=32
#SBATCH -C Haswell
#SBATCH -e error_file1024_8192.e

# Run the executable file
# and write the output into my_output_file
srun -n 1024 ./mat_mul_opt_noserial_8192 > ./results/out1024_8192
