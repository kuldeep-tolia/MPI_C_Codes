// MPI parallelized version to compute first derivative using explicit 2nd order central difference scheme
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

int main(int argc, char *argv[])	{

  int my_id, nprocs;
  MPI_Status status;

  int i, nx, local_n, local_xs, local_xe;

  double dx = 0.001;		/* set the delta-x */
  double xmin = -1.0;
  double xmax = 1.0;
  double U_left_bound = 0.0;
  double U_right_bound = 0.0;
  double x, temp;
  double *local_U, *local_dU;
  double *global_dU = NULL;
  double start_time, end_time;
  FILE *fptr;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

  start_time = MPI_Wtime();
  nx = (int)((xmax - xmin) / dx);
  local_n = nx / nprocs;

  /* assigning local start/end points for each process */
  if (my_id != nprocs-1)	{ 
    local_xs = my_id * local_n;
    local_xe = local_xs + local_n - 1;
  }
  else	{
    local_xs = my_id * local_n;
    local_xe = local_xs + local_n;
  }

  /* allocate memory */
  local_U = calloc(local_n+2, sizeof(double));
  local_dU = calloc(local_n+2, sizeof(double));

  /* calculate local-U_i before calculating derivatives */
  for(i = 1; i < local_n+1; i++)	{
    x = xmin + local_xs * dx + (i - 1) * dx;
    local_U[i] = x * tan(x);
  }

  if (my_id == nprocs-1)
    local_U[local_n+1] = 1.0 * tan(1.0);

  /* NOTE: since 2nd order CDS is implemented, it will require only one ghost point to store the boundary U value */
  /* communicating to left neighbour */
  if (my_id != 0)	
    MPI_Send(&local_U[1], 1, MPI_DOUBLE, my_id-1, 100, MPI_COMM_WORLD);
  if (my_id != nprocs-1)	{	
    MPI_Recv(&U_right_bound, 1, MPI_DOUBLE, my_id+1, 100, MPI_COMM_WORLD, &status);
    local_U[local_n+1] = U_right_bound;
  }

  /* communicating to right neighbour */
  if (my_id != nprocs-1)	
    MPI_Send(&local_U[local_n], 1, MPI_DOUBLE, my_id+1, 200, MPI_COMM_WORLD);
  if (my_id != 0)	{
    MPI_Recv(&U_left_bound, 1, MPI_DOUBLE, my_id-1, 200, MPI_COMM_WORLD, &status);
    local_U[0] = U_left_bound;
  }

  /* calculating first derivatives in each process */
  for(i = 1; i < local_n+1; i++)	{
    if ((my_id == 0) && (i == 1))	{
      local_dU[i] = (-3.0 * local_U[i] + 4.0 * local_U[i+1] - local_U[i+2]) / (2.0 * dx);
    }
    else	{
      local_dU[i] = (local_U[i+1] - local_U[i-1]) / (2.0 * dx);
    }
  }

  if (my_id == nprocs-1)
    local_dU[local_n+1] = (3.0 * local_U[local_n+1] - 4.0 * local_U[local_n] + local_U[local_n-1]) / (2.0 * dx);

  /* gathering locally computed first derivatives from each process into root process */
  if (my_id == 0)	{
    global_dU = calloc(nx+1, sizeof(double));
    MPI_Gather(&local_dU[1], local_n, MPI_DOUBLE, global_dU, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  }
  else	{
    MPI_Gather(&local_dU[1], local_n, MPI_DOUBLE, NULL, local_n, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  }

  if (my_id == nprocs-1)
    MPI_Send(&local_dU[local_n+1], 1, MPI_DOUBLE, 0, 300, MPI_COMM_WORLD);
  if (my_id == 0)	{
    MPI_Recv(&temp, 1, MPI_DOUBLE, nprocs-1, 300, MPI_COMM_WORLD, &status);
    global_dU[nx] = temp;
  }

  MPI_Barrier(MPI_COMM_WORLD);
  end_time = MPI_Wtime();

  /* writing results in an output file */
  if (my_id == 0)	{
    fptr = fopen("first_derivative_dx_0.001.txt", "w");
    for(i = 0; i < nx+1; i++)	{
      x = xmin + i * dx;
      fprintf(fptr, "%lf %lf %lf\n", x, tan(x) + x / (cos(x) * cos(x)), global_dU[i]);
    }
    fclose(fptr);
    free(global_dU);
  }  

  if (my_id == 0) printf("\nProgram running time = %lf, processes used = %d\n", end_time-start_time, nprocs);
  /* deallocating memory */
  free(local_U);
  free(local_dU);
    
  MPI_Finalize();  
  return 0;
}
