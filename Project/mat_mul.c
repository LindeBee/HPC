
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
	double start, end;

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
    int m_dim_nopad = 2048;
    int m_dim =  p_dim * (m_dim_nopad/p_dim + (m_dim_nopad % p_dim != 0));
    int *full_A;
    int *full_B;
    int *full_C;
    int *ser_C;

	full_A = malloc(m_dim*m_dim * sizeof(int));
	full_B = malloc(m_dim*m_dim * sizeof(int));
	full_C = malloc(m_dim*m_dim * sizeof(int));
	ser_C = malloc(m_dim*m_dim * sizeof(int));

    #define I_GMAT(R,C) ((R) * m_dim + (C))
    if (rank == 0){
        /*full_A = malloc(m_dim*m_dim*sizeof(int));
        full_B = malloc(m_dim*m_dim*sizeof(int));
        full_C = malloc(m_dim*m_dim*sizeof(int));
        ser_C = malloc(m_dim*m_dim*sizeof(int));*/
        for (int i = 0; i<m_dim; i++){
            for (int j = 0; j<m_dim; j++){
                if (i>=m_dim_nopad && j>=m_dim_nopad){
                    full_A[I_GMAT(i,j)] = 1;
                    full_B[I_GMAT(i,j)] = 1;
                } else if (i>=m_dim_nopad || j>=m_dim_nopad){
                    full_A[I_GMAT(i,j)] = 0;
                    full_B[I_GMAT(i,j)] = 0;
                } else {
                    full_A[I_GMAT(i,j)] = rand()%9+1;
                    full_B[I_GMAT(i,j)] = rand()%9+1;
                }
                full_C[I_GMAT(i,j)] = 0;
                ser_C[I_GMAT(i,j)] = 0;
            }
        }
        // //print matrix A
        // printf("A:\n");
        // for (int i = 0; i<m_dim_nopad; i++){
        //     for (int j = 0; j<m_dim_nopad; j++){
        //         printf("%3d", full_A[I_GMAT(i,j)]);
        //     }
        //     printf("\n");
        // }
        // printf("\n");
        // //print matrix B
        // printf("B:\n");
        // for (int i = 0; i<m_dim_nopad; i++){
        //     for (int j = 0; j<m_dim_nopad; j++){
        //         printf("%3d", full_B[I_GMAT(i,j)]);
        //     }
        //     printf("\n");
        // }
        // printf("\n");
    }
    
	// Root node serial code to test result and start performance measurements
    if (rank==0){
        printf("actual result:\n");
        for (int i = 0; i<m_dim_nopad; i++){
            for (int j = 0; j<m_dim_nopad; j++){
                for (int k = 0; k<m_dim_nopad; k++){
                    ser_C[I_GMAT(i,j)] += full_A[I_GMAT(i,k)]*full_B[I_GMAT(k,j)];
                }
                // printf("%5d", ser_C[I_GMAT(i,j)]);
            }
            // printf("\n");
        }

		// START TIMER
		start = MPI_Wtime();
    }

    

    // divide matrices into subblocks and subblocks over processes
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
	/*
	// All processes get sub matrices of A, B (and C)
	MPI_Win win;

	//A matrix
	if (rank == 0)
	{
		MPI_Win_create(full_A, m_dim*m_dim * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	}
	else
	{
		MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	}
	MPI_Get(loc_A, sub_dim*sub_dim, MPI_INT, 0, my_row*m_dim*sub_dim + my_col*sub_dim, 1, submat, win);
	MPI_Win_fence(0, win);
	MPI_Win_free(&win);
	
	//B matrix
	if (rank == 0)
	{
		MPI_Win_create(full_B, m_dim*m_dim * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	}
	else
	{
		MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	}
	MPI_Get(loc_B, sub_dim*sub_dim, MPI_INT, 0, my_row*m_dim*sub_dim + my_col*sub_dim, 1, submat, win);
	MPI_Win_fence(0, win);
	MPI_Win_free(&win);

	//C matrix
	if (rank == 0)
	{
		MPI_Win_create(full_C, m_dim*m_dim * sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	}
	else
	{
		MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
	}
	MPI_Get(loc_C, sub_dim*sub_dim, MPI_INT, 0, my_row*m_dim*sub_dim + my_col*sub_dim, 1, submat, win);
	MPI_Win_fence(0, win);
	MPI_Win_free(&win);
	*/
	
    // All processes get sub matrices of A, B (and C)
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
    
    //initialize cartesian communication system
    MPI_Comm cart_comm;
    MPI_Comm row_comm; 
    MPI_Comm col_comm; 
    int dims[2] = {p_dim,p_dim};
    int periodic[2] = {0,0};
    int remain_r[2] = {0,1};
    int remain_c[2] = {1,0};
    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, 0, &cart_comm);
    MPI_Cart_sub(cart_comm, remain_r, &row_comm);
    MPI_Cart_sub(cart_comm, remain_c, &col_comm);

    int *curr_A; //to store broadcasted A in without forgetting own A
    curr_A = malloc(sub_dim*sub_dim*sizeof(int));
    
    for (int r=0; r<p_dim; r++ ){
        // broadcast diagonal + i subblocks of A in horizontal direction
        memcpy(curr_A,loc_A,sub_dim*sub_dim*sizeof(int));
        MPI_Bcast(curr_A, sub_dim*sub_dim, MPI_INT, (my_row+r)%p_dim, row_comm);

        // multiply copied A subblocks into B subblocks
        for (int i = 0; i<sub_dim; i++){
            for (int j = 0; j<sub_dim; j++){
                for (int k = 0; k<sub_dim; k++){
                    loc_C[I_SMAT(i,j)] += curr_A[I_SMAT(i,k)]*loc_B[I_SMAT(k,j)];
                }
            }
        }

        // roll B blocks
        MPI_Sendrecv_replace(loc_B, sub_dim*sub_dim, MPI_INT, (my_row-1+p_dim)%p_dim, r,(my_row+1)%p_dim , r, col_comm, MPI_STATUS_IGNORE);
    }

    //DEBUG: print new local  matrices
    // printf("my rank: %d, my row: %d, my column:, %d\n", rank, my_row, my_col);
    // for (int i = 0; i<sub_dim; i++){
    //     for (int j = 0; j<sub_dim; j++){
    //         printf("%10d", loc_C[I_SMAT(i,j)]);
    //     }
    //     printf("\n");
    // }

    MPI_Win_create(full_C, m_dim*m_dim*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
    MPI_Win_fence(0, win);
    MPI_Put(loc_C, sub_dim*sub_dim ,MPI_INT, 0, my_row*m_dim*sub_dim+my_col*sub_dim ,1 , submat, win);
    MPI_Win_fence(0, win);
    MPI_Win_free(&win);

	free(loc_A);
	free(loc_B);
	free(loc_C);
	free(curr_A);

    // Root node final serial code
    if (rank ==0){
		//STOP TIMER
		end = MPI_Wtime();
		printf("Execution time: %f\n", end - start);

        // DEBUG: optional print results
        // printf("result:\n");
        // for (int i = 0; i<m_dim_nopad; i++){
        //     for (int j = 0; j<m_dim_nopad; j++){
        //         printf("%5d", full_C[I_GMAT(i,j)]);
        //     }
        //     printf("\n");
        // }

		// Check the correctness of the results of the distributed Fox's algorithm by comparing them to the naive serial implementation
        int stop = 0;
        for (int i = 0; i<m_dim_nopad; i++){
            for (int j = 0; j<m_dim_nopad; j++){
                if (full_C[I_GMAT(i,j)] != ser_C[I_GMAT(i,j)]){
                    stop =1;
                    printf("incorrect!\n");
                    break;
                }
            }
            if (stop){
                break;
            }
        }
        if (!stop){
            printf("correct!\n");
        }
    }

	free(full_A);
	free(full_B);
	free(full_C);
	free(ser_C);

    MPI_Finalize();

    return 0;
}