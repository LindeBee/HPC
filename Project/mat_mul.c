
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
    int m_dim = 4;
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

    // All processes get sub matrices of B (and C)
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

    // if (rank == proc[I_PROC(1,1)]){
    // //DEBUG: print sub-matrices
    // printf("my rank: %d, my row: %d, my column:, %d\n", rank, my_row, my_col);
    //     for (int i = 0; i<sub_dim; i++){
    //         for (int j = 0; j<sub_dim; j++){
    //             printf("%3d", loc_A[I_SMAT(i,j)]);
    //         }
    //         printf("\n");
    //     }
    // }
    
    //initialize cartesian communication system
    MPI_Comm cart_comm;
    MPI_Comm row_comm; 
    int dims[2] = {p_dim,p_dim};
    int periodic[2] = {0,0};
    int remain[2] = {0,1};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, 0, &cart_comm);
    MPI_Cart_sub(cart_comm, remain, &row_comm);

    int *curr_A; //to store broadcasted A in without forgetting own A
    curr_A = malloc(sub_dim*sub_dim*sizeof(int));
    
    for (int r=0; r<1; r++ ){
        // broadcast diagonal + i subblocks of A in horizontal direction
        memcpy(curr_A,loc_A,sub_dim*sub_dim*sizeof(int));
        MPI_Bcast(curr_A, sub_dim*sub_dim, MPI_INT, (my_row+r)%p_dim, row_comm);
        //DEBUG: print new local A matrices
        // printf("my rank: %d, my row: %d, my column:, %d\n", rank, my_row, my_col);
        // for (int i = 0; i<sub_dim; i++){
        //     for (int j = 0; j<sub_dim; j++){
        //         printf("%3d", curr_A[I_SMAT(i,j)]);
        //     }
        //     printf("\n");
        // }
        // works up to here!
        
        // TODO: multiply copied A subblocks into B subblocks
        for (int i = 0; i<sub_dim; i++){
            for (int j = 0; j<sub_dim; j++){
                for (int k = 0; k<sub_dim; k++){
                    loc_C[I_SMAT(i,j)] += curr_A[I_SMAT(i,k)]*loc_B[I_SMAT(k,j)];
                }
            }
        }

        //DEBUG: print new local C matrices
        printf("my rank: %d, my row: %d, my column:, %d\n", rank, my_row, my_col);
        for (int i = 0; i<sub_dim; i++){
            for (int j = 0; j<sub_dim; j++){
                printf("%3d", loc_C[I_SMAT(i,j)]);
            }
            printf("\n");
        }
        // TODO: roll B blocks
    }

    MPI_Finalize();

    return 0;
}