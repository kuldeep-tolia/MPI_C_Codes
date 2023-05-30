// MPI parallelized version to demonstrate usage of MPI_DIMS_CREATE
// reference link: https://rookiehpc.github.io/mpi/docs/mpi_dims_create/index.html
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 Illustrate how to decompose MPI processes in a cartesian grid.
 This application is to be run with 12 processes. It attempts to
 decompose these 12 MPI processes in 3 dimensions.
*/

int main(int argc, char *argv[])	{

  int my_id, nprocs;
  MPI_Status status;
	
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  if(nprocs != 12)	{
    printf("This program is meant to be run with 12 MPI processes.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  if (my_id == 0)	{
    printf("Attempting to decompose %d processes in 3 dimensions.\n", nprocs);

    // Give total freedom to MPI and auto-allocate
    int dimsA[3] = {0, 0, 0};
    printf("Restriction (%d, %d, %d) gave following decomposition: ", dimsA[0], dimsA[1], dimsA[2]);
    MPI_Dims_create(nprocs, 3, dimsA);
    printf("(%d, %d, %d).\n", dimsA[0], dimsA[1], dimsA[2]);

    // Give restriction of 6 processes in 1st dimension
    int dimsB[3] = {6, 0, 0};
    printf("Restriction (%d, %d, %d) gave following decomposition: ", dimsB[0], dimsB[1], dimsB[2]);
    MPI_Dims_create(nprocs, 3, dimsB);
    printf("(%d, %d, %d).\n", dimsB[0], dimsB[1], dimsB[2]);

    // Give restriction of 4 processes in 2nd dimension and 1 process in 3rd dimension
    int dimsC[3] = {0, 4, 1};
    printf("Restriction (%d, %d, %d) gave following decomposition: ", dimsC[0], dimsC[1], dimsC[2]);
    MPI_Dims_create(nprocs, 3, dimsC);
    printf("(%d, %d, %d).\n", dimsC[0], dimsC[1], dimsC[2]);
  }
      
  MPI_Finalize();	
  return 0;
}
