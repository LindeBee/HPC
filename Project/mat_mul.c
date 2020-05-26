
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
    // TODO: initialise matrix A, B, (C=zeros(A(1), B(0)))

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Store processes in matrix
    dim = sqrt(size);
    int *proc = NULL;
    if (rank == 0) { 
        proc = malloc(dim*dim*sizeof(int))
        int p = 0;
        for (int i = 0; i<dim; i++){
            for (int j = 0; j<dim; j++){
                proc[i][j] = p;
                p++;
            }
        }
    }

    // TODO: pad matrix if necessary


    // TODO: divide matrices into subblocks.
    

    for (int i=0; i<sub_x; i++ ){
        // TODO: broadcast diagonal + i subblocks of A in horizontal direction

        // TODO: multiply copied A subblocks into B subblocks

        // TODO: roll B blocks
    }

    MPI_Finalize();

    return 0;
}