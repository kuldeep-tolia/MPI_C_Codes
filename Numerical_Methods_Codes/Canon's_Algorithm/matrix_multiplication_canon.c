// MPI parallelized version of matrix(nxn) multiplication using Canon's algorithm
// Assumptions:
// A and B matrices are square matrices and the number of processors used should be a square number.
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

void allocate_memory(double **local_A_pp, double **local_B_pp, double **local_C_pp, double **buffer_pp, int local_N, MPI_Comm comm)	{

  *local_A_pp = malloc(local_N * local_N * sizeof(double));
  *local_B_pp = malloc(local_N * local_N * sizeof(double));
  *buffer_pp = malloc(local_N * local_N * sizeof(double));
  *local_C_pp = calloc(local_N * local_N, sizeof(double));
  
  return;
}

void populate_matrices(double *local_A, double *local_B, double *local_C, int local_N, MPI_Comm comm)	{

  int i, j;
  unsigned int iseed = 0;
  srand(iseed);

  for(i = 0; i < local_N; i++)	{
    for(j = 0; j < local_N; j++)	{
      local_A[i*local_N+j] = 1.0; /* 4.0 - (int)(2.0 * rand() / (RAND_MAX + 1.0)); */ // 1.0 is used for sanity check, uncomment to use a random number generator 
      local_B[i*local_N+j] = 1.0; /* 4.0 - (int)(2.0 * rand() / (RAND_MAX + 1.0)); */ // 1.0 is used for sanity check, uncomment to use a random number generator
      local_C[i*local_N+j] = 0.0;
    }
  }
  
  return;
}

void print_matrix(double *local_mat, int local_N)	{

  for(int i = 0; i < local_N; i++)	{
    for(int j = 0; j < local_N; j++)	{
      printf("%lf ", local_mat[i*local_N+j]);
    }
    printf("\n");
  }
  
  return;
}
 
void deallocate_memory(double *local_A, double *local_B, double *local_C, double *buffer, MPI_Comm comm)	{

  free(local_A);
  free(local_B);
  free(buffer);
  free(local_C);
  return;
}

void matrix_mult(double *local_A, double *local_B, double *local_C, int local_N)	{

  int i, j, k;

  for(i = 0; i < local_N; i++)
    for(j = 0; j < local_N; j++)
      for(k = 0; k < local_N; k++)
	local_C[i*local_N+j] += local_A[i*local_N+k] * local_B[k*local_N+j];

  return;
}

int main(int argc, char *argv[])	{

  int my_id, nprocs;
  MPI_Status status;
  MPI_Comm cannon_comm;

  int i, j, local_N;
  int dims[2], periods[2];
  int left, right, up, down;

  int N = 16;			/* size of the global matrix */

  double *local_A, *local_B, *local_C, *buffer, *temp;
  double* global_C = NULL;
  double start_time, end_time;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  /* initialize new communicator */
  dims[0] = dims[1] = 0;	/* allowing MPI to auto-allocate the process in each direction */
  MPI_Dims_create(nprocs, 2, dims);
  if (dims[0] != dims[1])	{
    if (my_id == 0) printf("\nThe number of processes must be a square number.\n");
    MPI_Finalize();
    return 0;
  }

  periods[0] = periods[1] = 1;	/* setting periodicity in each direction for wraparound */

  local_N = N / dims[0];

  allocate_memory(&local_A, &local_B, &local_C, &buffer, local_N, MPI_COMM_WORLD);
  
  populate_matrices(local_A, local_B, local_C, local_N, MPI_COMM_WORLD);
  
  /* creating new communicator */ 
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 1, &cannon_comm); /* create a new communicator with cartesian topology */
  
  /* obtain ranks of neighbouring processes */
  MPI_Cart_shift(cannon_comm, 0, 1, &left, &right);
  MPI_Cart_shift(cannon_comm, 1, 1, &up, &down);
  
  /* shift cycle of cannon algorithm begins */
  start_time = MPI_Wtime();
  for(i = 0; i < dims[0]; i++)	{
  	matrix_mult(local_A, local_B, local_C, local_N);
  	if (i == dims[0]-1) break;
  	
  	/* communication calls */
  	MPI_Sendrecv(local_A, local_N*local_N, MPI_DOUBLE, left, 1, buffer, local_N*local_N, MPI_DOUBLE, right, 1, cannon_comm, &status);
  	temp = buffer;
  	buffer = local_A;
  	local_A = temp;
  	MPI_Sendrecv(local_B, local_N*local_N, MPI_DOUBLE, up, 2, buffer, local_N*local_N, MPI_DOUBLE, down, 2, cannon_comm, &status);
  	temp = buffer;
  	buffer = local_B;
  	local_B = temp;
  }  	
  
  MPI_Barrier(cannon_comm);
  end_time = MPI_Wtime();
  
  /* printing random element of the obtained matrix from each process */
  /* as the values assigned were 1.0, the obtained matrix-C should have elements equal to N */
  //printf("\nCheck printing local_C values: C[%d][%d] = %lf from process = %d\n", local_N/2, local_N/2, local_C[(local_N/2 * local_N) + (local_N/2)], my_id);
  
  if (my_id == 0)	{	// user can turn off/on the comment for printing the obtained global matrix for smaller matrices
    global_C = malloc(N * N * sizeof(double));
    MPI_Gather(local_C, local_N*local_N, MPI_DOUBLE, global_C, local_N*local_N, MPI_DOUBLE, 0, cannon_comm);
  }
  else	{
    MPI_Gather(local_C, local_N*local_N, MPI_DOUBLE, NULL, local_N*local_N, MPI_DOUBLE, 0, cannon_comm);
  }

  MPI_Barrier(cannon_comm);

  if (my_id == 0)	{
    printf("\n---------Printing global matrix-C---------\n");	
    for(i = 0; i < N; i++)	{
      for(j = 0; j < N; j++)	{
	printf("%lf ", global_C[i*N+j]);
      }
      printf("\n");
    }

    free(global_C);
  }	
  
  if (my_id == 0) printf("\nProgram running time = %lf, processes used = %d\n", end_time-start_time, nprocs);
  
  deallocate_memory(local_A, local_B, local_C, buffer, MPI_COMM_WORLD);  
  
  MPI_Finalize();  
  return 0;
}
