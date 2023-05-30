// MPI parallelized version of matrix(mxn) addition
// row-wise block parallelization
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

void allocate_memory(double **local_A_pp, double **local_B_pp, double **local_C_pp, int local_m, int n, MPI_Comm comm)	{
  *local_A_pp = malloc(local_m * n * sizeof(double));
  *local_B_pp = malloc(local_m * n * sizeof(double));
  *local_C_pp = malloc(local_m * n * sizeof(double));
  return;
}

void populate_matrices(double *local_A, double *local_B, int m, int local_m, int n, int my_id, MPI_Comm comm)	{
  double* matA = NULL;
  double* matB = NULL;
  int i, j;
   
  if (my_id == 0)	{
    matA = malloc(m * n * sizeof(double));
    matB = malloc(m * n * sizeof(double));    

    for(i = 0; i < m; i++)	{
      for(j = 0; j < n; j++)	{
	matA[i*n+j] = 5.0;	/* for simplicity and sanity check, it is kept as 5.0 */
	matB[i*n+j] = 5.0;	/* for simplicity and sanity check, it is kept as 5.0 */
      }
    }
    
    MPI_Scatter(matA, local_m*n, MPI_DOUBLE, local_A, local_m*n, MPI_DOUBLE, 0, comm);
    MPI_Scatter(matB, local_m*n, MPI_DOUBLE, local_B, local_m*n, MPI_DOUBLE, 0, comm);
    free(matA);
    free(matB);
  }
  else	{
    MPI_Scatter(NULL, local_m*n, MPI_DOUBLE, local_A, local_m*n, MPI_DOUBLE, 0, comm);
    MPI_Scatter(NULL, local_m*n, MPI_DOUBLE, local_B, local_m*n, MPI_DOUBLE, 0, comm);
  }   
  return;
}
    
void mat_add(double* local_A, double* local_B, double* local_C, int m, int local_m, int n, int my_id, MPI_Comm comm)	{
  double* global_C = NULL;
  int i, j;

  for(i = 0; i < local_m; i++)	
    for(j = 0; j < n; j++)	
      local_C[i*n+j] = local_A[i*n+j] + local_B[i*n+j];

  /* if (my_id == 0)	{	// user can turn off the comment for printing the obtained global matrix
    global_C = malloc(m * n * sizeof(double));
    MPI_Gather(local_C, local_m*n, MPI_DOUBLE, global_C, local_m*n, MPI_DOUBLE, 0, comm);
  }
  else	{
    MPI_Gather(local_C, local_m*n, MPI_DOUBLE, NULL, local_m*n, MPI_DOUBLE, 0, comm);
  }

  MPI_Barrier(comm);

  if (my_id == 0)	{	
    for(i = 0; i < m; i++)	{
      for(j = 0; j < n; j++)	{
	printf("%lf ", global_C[i*n+j]);
      }
      printf("\n");
    }

    free(global_C);
  }	*/
  return;
}

int main(int argc, char *argv[])	{

  double *local_A, *local_B, *local_C;
  int my_id, nprocs;
  int m, local_m, n;
  double start, end;
  MPI_Status status;
  MPI_Comm comm;

  m = 10240;			/* number of rows */
  n = 10240;			/* number of columns */
  
  start = MPI_Wtime();
  MPI_Init(&argc, &argv);
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &nprocs);
  MPI_Comm_rank(comm, &my_id);

  local_m = m / nprocs;		/* m > 0 and should be evenly divisible by nprocs */
  allocate_memory(&local_A, &local_B, &local_C, local_m, n, comm);
  populate_matrices(local_A, local_B, m, local_m, n, my_id, comm);
  mat_add(local_A, local_B, local_C, m, local_m, n, my_id, comm);
	
  /* printing random element of the obtained matrix from each process */
  /* as the values assigned were 5.0, the obtained matrix-C should be equal 10.0 */
  printf("Sanity check printing local_C values: C[%d][%d] = %lf from process = %d\n", local_m/2, n/2, local_C[(local_m/2 * n) + (n/2)], my_id);
  
  MPI_Finalize();
  end = MPI_Wtime();

  if (my_id == 0) printf("\nProgram running time = %lf, processes used = %d\n", end-start, nprocs);
  return 0;
}
