
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define SEED     921
#define NUM_ITER 1000000000

int main(int argc, char* argv[])
{
    int count = 0, intermediate = 0;
    double x, y, z, pi, start, end;
    
	int rank, size, i, provided;

	MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int *counts = NULL;

	if (rank == 0)
	{
		counts = malloc((size) * sizeof(int));
	}
	

    srand(SEED * rank); // Important: Multiply SEED by "rank" when you introduce MPI!
    
	start = MPI_Wtime();

    // Calculate PI following a Monte Carlo method
    for (int iter = 0; iter < NUM_ITER/size; iter++)
    {
        // Generate random (X,Y) points
        x = (double)random() / (double)RAND_MAX;
        y = (double)random() / (double)RAND_MAX;
        z = sqrt((x*x) + (y*y));
        
        // Check if point is in unit circle
        if (z <= 1.0)
        {
            count++;
        }
    }

	MPI_Gather(&count, 1, MPI_INT, counts, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (rank == 0)
	{
		count = 0;
		for (int i = 0; i < size; i++)
		{
			count += counts[i];
		}
		// Estimate Pi and display the result
		pi = ((double)count / (double)NUM_ITER) * 4.0;
		end = MPI_Wtime();
		printf("The result is %f\tExecution time is %f\n", pi, end - start);
	}

	MPI_Finalize();

    return 0;
}

