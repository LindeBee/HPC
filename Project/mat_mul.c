
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <mpi.h>

#define SEED     921

int main(int argc, char* argv[])
{
    int rank, size, i, provided;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_SINGLE, &provided);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Store processes in matrix
    int p_dim = sqrt(size);
    int my_row, my_col;
    int *proc = NULL;
    proc = malloc(p_dim*p_dim*sizeof(int));
    #define I_PROC(R,C) ((R) * p_dim + (C))
    int p = 0;
    for (int i = 0; i<p_dim; i++){
        for (int j = 0; j<p_dim; j++){
            proc[I_PROC(i,j)] = p;
            if (p == rank){ // make sure every process knows where in the matrix it is
                my_row = i;
                my_col = j;
            }
            p++;
        }
    }

    // initialise matrix A, B, (C=zeros(A(1), B(0)))
    int m_dim = 8;
    int *full_A = NULL;
    int *full_B = NULL;
    int *full_C = NULL;
    full_A = malloc(m_dim*m_dim*sizeof(int));
    full_B = malloc(m_dim*m_dim*sizeof(int));
    full_C = malloc(m_dim*m_dim*sizeof(int));
    #define I_GMAT(R,C) ((R) * m_dim + (C))
    if (rank == 0){
        for (int i = 0; i<m_dim; i++){
            for (int j = 0; j<m_dim; j++){
                full_A[I_GMAT(i,j)] = rand()%9+1;
                full_B[I_GMAT(i,j)] = rand()%9+1;
                full_C[I_GMAT(i,j)] = 0;
            }
        }
        //DEBUG: print matrix A
        for (int i = 0; i<m_dim; i++){
            for (int j = 0; j<m_dim; j++){
                printf("%3d", full_A[I_GMAT(i,j)]);
            }
            printf("\n");
        }
        printf("\n");
        //DEBUG: print matrix B
        for (int i = 0; i<m_dim; i++){
            for (int j = 0; j<m_dim; j++){
                printf("%3d", full_B[I_GMAT(i,j)]);
            }
            printf("\n");
        }
        printf("\n");
    }
    
    // TODO: pad matrix if necessary
    // divide matrices into subblocks and  subblocks over processes
    int sub_dim = m_dim/p_dim;
    #define I_SMAT(R,C) ((R) * sub_dim + (C))
    int *loc_A;
    int *loc_B;
    int *loc_C;
    loc_A = malloc(sub_dim*sub_dim*sizeof(int));
    loc_B = malloc(sub_dim*sub_dim*sizeof(int));
    loc_C = malloc(sub_dim*sub_dim*sizeof(int));
    
    //define MPI type for single submatrix
    MPI_Datatype submat;
    MPI_Type_vector(sub_dim, sub_dim, m_dim, MPI_INT, &submat); //it ended up being in MPI datatypes lecture
    MPI_Type_commit(&submat);

    // All processes get sub matrices
    // NOTE: Is there a more efficient way to do this??
    MPI_Win win;
    MPI_Win_create(full_A, m_dim*m_dim*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
    MPI_Get(loc_A, sub_dim*sub_dim ,MPI_INT, 0, my_row*m_dim*sub_dim+my_col*sub_dim ,1 , submat, win);
    MPI_Win_fence(0, win);
    MPI_Win_free(&win);
    MPI_Win_create(full_B, m_dim*m_dim*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
    MPI_Get(loc_B, sub_dim*sub_dim ,MPI_INT, 0, my_row*m_dim*sub_dim+my_col*sub_dim ,1 , submat, win);
    MPI_Win_fence(0, win);
    MPI_Win_free(&win);
    MPI_Win_create(full_C, m_dim*m_dim*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
    MPI_Get(loc_C, sub_dim*sub_dim ,MPI_INT, 0, my_row*m_dim*sub_dim+my_col*sub_dim ,1 , submat, win);
    MPI_Win_fence(0, win);
    MPI_Win_free(&win);

    if (rank == proc[I_PROC(1,1)]){
    //DEBUG: print sub-matrices
    // printf("my rank: %d, my row: %d, my column:, %d\n", rank, my_row, my_col);
    //     for (int i = 0; i<sub_dim; i++){
    //         for (int j = 0; j<sub_dim; j++){
    //             printf("%3d", loc_A[I_SMAT(i,j)]);
    //         }
    //     }
    // }
    
    // works up to here!

    // // for (int i=0; i<sub_dim; i++ ){
    //     // TODO: broadcast diagonal + i subblocks of A in horizontal direction

    //     // TODO: multiply copied A subblocks into B subblocks

    //     // TODO: roll B blocks
    // // }

    MPI_Finalize();

    return 0;
}