// MPI parallelized version of matrix(mxn) - vector(nx1) multiplication
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

void read_dimensions(int *m_p, int* local_m_p, int* n_p, int* local_n_p, int my_id, int nprocs, MPI_Comm comm)	{
  if (my_id == 0)	{
    printf("Enter number of rows:\n");		// m should be evenly divisible by nprocs and m > 0
    scanf("%d", m_p);
    printf("Enter number of columns:\n");	// n should be evenly divisible by nprocs and n > 0
    scanf("%d", n_p);
    printf("\n");
  }
  
  MPI_Bcast(m_p, 1, MPI_INT, 0, comm);
  MPI_Bcast(n_p, 1, MPI_INT, 0, comm);

  if (*m_p <= 0 || *n_p <= 0 || *m_p%nprocs != 0 || *n_p%nprocs != 0)	{ /* check for any errors */
    if(my_id == 0) printf("\nPlease enter correct dimensions or number of MPI processes. Exiting!!\n");
    MPI_Finalize();
    exit(0);
  }
  
  *local_m_p = *m_p / nprocs;
  *local_n_p = *n_p / nprocs;
  return;
}

void allocate_memory(double **local_A_pp, double **local_x_pp, double **local_b_pp, int local_m, int n, int local_n, MPI_Comm comm)	{
  *local_A_pp = malloc(local_m * n * sizeof(double));
  *local_x_pp = malloc(local_n * sizeof(double));
  *local_b_pp = malloc(local_m * sizeof(double));
  return;
}

void populate_matrices(double *local_A, double *local_x, int m, int local_m, int n, int local_n, int my_id, MPI_Comm comm)	{
  double* matA = NULL;
  double* vec = NULL;
  int i, j;
   
  if(my_id == 0)	{
    matA = malloc(m * n * sizeof(double));
    vec = malloc(n * sizeof(double));    
    for(i = 0; i < m; i++)	
      for(j = 0; j < n; j++)	
	matA[i*n+j] = 1.0;	/* for simplicity and sanity check, it is kept as 1.0 */
    for(i = 0; i < n; i++)	vec[i] = 1.0;
    MPI_Scatter(matA, local_m*n, MPI_DOUBLE, local_A, local_m*n, MPI_DOUBLE, 0, comm);
    MPI_Scatter(vec, local_n, MPI_DOUBLE, local_x, local_n, MPI_DOUBLE, 0, comm);
    free(matA);
    free(vec);
  }
  else	{
    MPI_Scatter(NULL, local_m*n, MPI_DOUBLE, local_A, local_m*n, MPI_DOUBLE, 0, comm);
    MPI_Scatter(NULL, local_n, MPI_DOUBLE, local_x, local_n, MPI_DOUBLE, 0, comm);
  }   
  return;
}
    
void matvec_multiply(double* local_A, double* local_x, double* local_b, int local_m, int local_n, int n, MPI_Comm comm)	{
  double* global_x = NULL;
  int i, j;

  global_x = malloc(n * sizeof(double));
  MPI_Allgather(local_x, local_n, MPI_DOUBLE, global_x, local_n, MPI_DOUBLE, comm);

  for(i = 0; i < local_m; i++)	{
    local_b[i] = 0.0;
    for(j = 0; j < n; j++)	
      local_b[i] += local_A[i*n+j] * global_x[j];
  }

  free(global_x);
  return;
}

int main(int argc, char *argv[])	{

  double *local_A, *local_x, *local_b;
  int my_id, nprocs;
  int m, local_m, n, local_n;
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
  local_n = n / nprocs;		/* n > 0 and should be evenly divisible by nprocs */
  // read_dimensions(&m, &local_m, &n, &local_n, my_id, nprocs, comm);   // user can uncomment this procedure if wish to
  allocate_memory(&local_A, &local_x, &local_b, local_m, n, local_n, comm);
  populate_matrices(local_A, local_x, m, local_m, n, local_n, my_id, comm);
  matvec_multiply(local_A, local_x, local_b, local_m, local_n, n, comm);
	
  /* printing random element of the obtained vector from each process*/
  /* as the values assigned were 1.0, the obtained vector-b should be equal to #columns, i.e., n */
  printf("Sanity check printing local_b values: b[%d] = %lf from process = %d\n", local_m/2, local_b[local_m/2], my_id);
  
  MPI_Finalize();
  end = MPI_Wtime();

  if (my_id == 0) printf("\nProgram running time = %lf, processes used = %d\n", end-start, nprocs);
  return 0;
}
