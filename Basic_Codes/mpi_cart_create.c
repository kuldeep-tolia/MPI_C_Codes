// MPI parallelized version to demonstrate usage of MPI_CART_CREATE
// reference link: https://rookiehpc.github.io/mpi/docs/mpi_cart_create/index.html
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>

/*
 This program illustrates how to create a communicator representing a 2D torus topology.
*/

int main(int argc, char *argv[])	{

  int my_id, nprocs;
  MPI_Status status;
	
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  
  int dims[2] = {0, 0};		/* ask MPI to auto-allocate processes in 2D */
  MPI_Dims_create(nprocs, 2, dims);
  
  int periodic[2] = {true, true}; /* making both dimensions periodic directions */
  int reorder = true;		 /* Let MPI assign process ranks if it deems necessary */

  MPI_Comm new_comm;
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periodic, reorder, &new_comm);
  MPI_Comm_rank(new_comm, &my_id); /* get the process rank in the new communicator */

  int my_coords[2];
  MPI_Cart_coords(new_comm, my_id, 2, my_coords); /* get the coordinates of a process in the new communicator with 2D cartesian topology */

  printf("MPI process = %d, is located at (%d, %d).\n", my_id, my_coords[0], my_coords[1]);
      
  MPI_Finalize();	
  return 0;
}
