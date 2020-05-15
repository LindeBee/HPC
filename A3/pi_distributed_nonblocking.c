
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

	MPI_Request *requests = malloc((size-1) * sizeof(MPI_Request));
	int *counts = malloc((size-1) * sizeof(int));

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

	if (rank == 0)
	{
		for (int i = 1; i < size; i++)
		{
			MPI_Irecv(&counts[i-1], 1, MPI_INT, i, 0, MPI_COMM_WORLD, &requests[i-1]);
		}

		MPI_Waitall(size-1, requests, MPI_STATUSES_IGNORE);
		for (int i = 0; i < size - 1; i++)
		{
			count += counts[i];
		}

		// Estimate Pi and display the result
		pi = ((double)count / (double)NUM_ITER) * 4.0;
		end = MPI_Wtime();
		printf("The result is %f\tExecution time is %f\n", pi, end - start);
	}
	else
	{
		MPI_Send(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	free(requests);
	free(counts);
	MPI_Finalize();

    return 0;
}

