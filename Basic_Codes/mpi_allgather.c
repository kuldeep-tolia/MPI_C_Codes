// MPI parallelized version to demonstrate usage of MPI_ALLGATHER
// reference link: https://rookiehpc.github.io/mpi/docs/mpi_allgather/index.html
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 This program is meant to be run with 3 processes. Every MPI
 process begins with a value, then every MPI process collects the entirety of
 the data gathered and prints them. It can be visualised as follows:
  +-----------+  +-----------+  +-----------+
  | Process 0 |  | Process 1 |  | Process 2 |
  +-+-------+-+  +-+-------+-+  +-+-------+-+
    | Value |      | Value |      | Value |
    |   0   |      |  100  |      |  200  |
    +-------+      +-------+      +-------+
        |________      |      ________|
                 |     |     | 
              +-----+-----+-----+
              |  0  | 100 | 200 |
              +-----+-----+-----+
              |   Each process  |
*/

int main(int argc, char *argv[])	{

  int my_id, nprocs;
  MPI_Status status;
	
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  if(nprocs != 3)	{
    printf("This program is meant to be run with 3 MPI processes.\n");
    MPI_Abort(MPI_COMM_WORLD, EXIT_FAILURE);
  }

  int my_value = my_id * 100;
  printf("Process = %d, my_value = %d.\n", my_id, my_value);

  int buffer[3];
  MPI_Allgather(&my_value, 1, MPI_INT, buffer, 1, MPI_INT, MPI_COMM_WORLD);
  printf("Values collected on process-%d: %d, %d, %d.\n", my_id, buffer[0], buffer[1], buffer[2]);
  
  MPI_Finalize();	
  return 0;
}
