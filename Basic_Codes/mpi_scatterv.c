// MPI parallelized version to demonstrate usage of MPI_SCATTERV
// reference link: https://rookiehpc.github.io/mpi/docs/mpi_scatterv/index.html
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 A process is designed as root and begins with a buffer containig all
 values, and prints them. It then dispatches these values to all the processes
 in the same communicator. Other process just receive the dispatched value(s)
 meant for them. Finally, everybody prints the value received. This
 application is designed to cover all cases:
 - Different send counts
 - Different displacements
 This application is meant to be run with 3 processes.
        +-----------------------------------------+
        |                Process 0                |
        +-----+-----+-----+-----+-----+-----+-----+
        | 100 |  0  | 101 | 102 |  0  |  0  | 103 |
        +-----+-----+-----+-----+-----+-----+-----+
          |            |     |                |
          |            |     |                |
          |            |     |                |
          |            |     |                |
          |            |     |                |
          |            |     |                |
  +-----------+ +-------------------+ +-----------+
  | Process 0 | |    Process 1      | | Process 2 |
  +-+-------+-+ +-+-------+-------+-+ +-+-------+-+
    | Value |     | Value | Value |     | Value |
    |  100  |     |  101  |  102  |     |  103  |
    +-------+     +-------+-------+     +-------+ 
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

  int root_rank = 0;
  switch(my_id)	{
  case 0:	{
    int my_value;
    int buffer[7] = {100, 0, 101, 102, 0, 0, 103};
    int counts[3] = {1, 2, 1};
    int displacements[3] = {0, 2, 6};
    printf("Values in the buffer of root process:\n");
    for(int i = 0; i < 7; i++)	{
      printf("%d ", buffer[i]);
    }
    printf("\n");
    MPI_Scatterv(buffer, counts, displacements, MPI_INT, &my_value, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
    printf("Process-%d received my_value = %d.\n", my_id, my_value);
    break;
  }
  case 1:	{
    int my_value[2];
    MPI_Scatterv(NULL, NULL, NULL, MPI_INT, my_value, 2, MPI_INT, root_rank, MPI_COMM_WORLD);
    printf("Process-%d received my_value = %d, %d.\n", my_id, my_value[0], my_value[1]);
    break;
  }
  case 2:	{
    int my_value;
    MPI_Scatterv(NULL, NULL, NULL, MPI_INT, &my_value, 1, MPI_INT, root_rank, MPI_COMM_WORLD);
    printf("Process-%d received my_value = %d.\n", my_id, my_value);
    break;
  }
  }
      
  MPI_Finalize();	
  return 0;
}
