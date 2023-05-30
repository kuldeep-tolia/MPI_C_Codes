// MPI parallelized version to demonstrate usage of MPI_REDUCE
// reference link: https://rookiehpc.github.io/mpi/docs/mpi_reduce/index.html
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
  This application consists of a sum reduction; every MPI process
  sends its rank for reduction before the sum of these ranks is stored in the
  root MPI process. It can be visualised as follows, with MPI process 0 as
  root:
  +-----------+ +-----------+ +-----------+ +-----------+
  | Process 0 | | Process 1 | | Process 2 | | Process 3 |
  +-+-------+-+ +-+-------+-+ +-+-------+-+ +-+-------+-+
    | Value |     | Value |     | Value |     | Value |
    |   0   |     |   1   |     |   2   |     |   3   |
    +-------+     +-------+     +-------+     +-------+
             \         |           |         /
              \        |           |        /
               \       |           |       /
                \      |           |      /
                 +-----+-----+-----+-----+
                             |
                         +---+---+
                         |  SUM  |
                         +---+---+
                             |
                         +---+---+
                         |   6   |
                       +-+-------+-+
                       | Process 0 |
                       +-----------+
*/

int main(int argc, char *argv[])	{

  int my_id, nprocs;
  MPI_Status status;
	
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  int root_rank = 0;
  if(nprocs != 4)	{
    printf("This program is meant to be run with 4 MPI processes.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  int reduction_result = 0;
  MPI_Reduce(&my_id, &reduction_result, 1, MPI_INT, MPI_SUM, root_rank, MPI_COMM_WORLD);

  if(my_id == root_rank)	{
    printf("The sum of all ranks is %d.\n", reduction_result);
  }
  
  MPI_Finalize();	
  return 0;
}
