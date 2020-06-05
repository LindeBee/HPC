#!/bin/bash -l

# The name of the script is matmul_batch_job
#SBATCH -J matmulBatchRun144
# Only 1 hour wall-clock time will be given to this job
#SBATCH -t 00:10:00
#SBATCH -A edu20.DD2356
# Number of nodes
#SBATCH --nodes=5
#SBATCH --ntasks-per-node=32
#SBATCH -C Haswell
#SBATCH -e error_file144.e

# Run the executable file
# and write the output into my_output_file
srun -n 144 ./mat_mul > ./out144
