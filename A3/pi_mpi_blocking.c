#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <sys/time.h>


#define SEED     921
#define NUM_ITER 1280000000


int main(int argc, char* argv[])
{
    
    int count = 0;
    double x, y, z, pi;
    int rank, size, provided, status, count_in;
    double t1, t2; //timers



    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    srand(SEED*rank); // Important: Multiply SEED by "rank" when you introduce MPI!
    int n_iters = NUM_ITER/size;
    t1 = MPI_Wtime();
    // Calculate PI following a Monte Carlo method
    for (int iter = 0; iter < n_iters; iter++)
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

    int nodes = size;
    int step = 1;
    while (nodes > 1){
        // for (int i = 0; i < size; i += step){
            if (rank%(2*step) == step){
                // printf("Rank %d sending %d to %d\n", rank, count, rank-step);
                MPI_Send(&count, 1, MPI_DOUBLE, (rank-step), nodes, MPI_COMM_WORLD);
            }
            else if (rank%(2*step) == 0) {
                MPI_Recv(&count_in, 1, MPI_DOUBLE, (rank+step), nodes, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                // printf("Rank %d receiving %d from %d\n", rank, count_in, rank+step);
                count = count+count_in;
            }
        // }
        step = step * 2;
        nodes = nodes/2;
    }

    if (rank == 0){
        // Estimate Pi and display the result
        pi = ((double)count / (double)NUM_ITER) * 4.0;
        t2 = MPI_Wtime();
        printf("The result is %lf\n", pi);
        printf ("time: %lf\n", t2-t1);
    }

    MPI_Finalize();


    

    return 0;
}

