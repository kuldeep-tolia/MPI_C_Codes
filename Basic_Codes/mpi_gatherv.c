// MPI parallelized version to demonstrate usage of MPI_GATHERV
// reference link: https://rookiehpc.github.io/mpi/docs/mpi_gatherv/index.html
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
 This application is meant to be run with 3 processes. Every MPI process begins
 with a value, the MPI process 0 will gather all these values and print them.
 The example is designed to cover all cases:
 - Different displacements
 - Different receive counts
 It can be visualised as follows: 
  +-----------+ +-----------+ +-------------------+ 
  | Process 0 | | Process 1 | |     Process 2     |
  +-+-------+-+ +-+-------+-+ +-+-------+-------+-+
    | Value |     | Value |     | Value | Value |
    |  100  |     |  101  |     |  102  |  103  |
    +-------+     +-------+     +-------+-------+
       |                |            |     |
       |                |            |     |
       |                |            |     |
       |                |            |     |
       |                |            |     |
       |                |            |     |
    +-----+-----+-----+-----+-----+-----+-----+
    | 100 |  0  |  0  | 101 |  0  | 102 | 103 |
    +-----+-----+-----+-----+-----+-----+-----+
    |                Process 0                |
    +-----------------------+-----+-----+-----+
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
    int my_value = 100;
    int counts[3] = {1, 1, 2};
    int displacements[3] = {0, 3, 5};
    int* buffer = (int*)calloc(7, sizeof(int));
    printf("Process-%d, my_value = %d.\n", my_id, my_value);
    MPI_Gatherv(&my_value, 1, MPI_INT, buffer, counts, displacements, MPI_INT, root_rank, MPI_COMM_WORLD);
    printf("Values gathered in the buffer on process-%d:\n", my_id);
    for(int i = 0; i < 7; i++)	{
      printf("%d ", buffer[i]);
    }
    printf("\n");
    free(buffer);
    break;
  }
  case 1:	{
    int my_value = 101;
    printf("Process-%d, my_value = %d.\n", my_id, my_value);
    MPI_Gatherv(&my_value, 1, MPI_INT, NULL, NULL, NULL, MPI_INT, root_rank, MPI_COMM_WORLD);
    break;
  }
  case 2:	{
    int my_value[2] = {102, 103};
    printf("Process-%d, my_value = %d, %d.\n", my_id, my_value[0], my_value[1]);
    MPI_Gatherv(my_value, 2, MPI_INT, NULL, NULL, NULL, MPI_INT, root_rank, MPI_COMM_WORLD);
    break;
  }
  }
      
  MPI_Finalize();	
  return 0;
}
