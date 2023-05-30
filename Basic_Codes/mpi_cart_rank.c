// MPI parallelized version to demonstrate usage of MPI_CART_RANK
// reference link: http://mpi.deino.net/mpi_functions/MPI_Cart_rank.html
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <stdbool.h>

/*
 A 2D torus topology of 12 processes with 4x3 grid.
*/

int main(int argc, char *argv[])	{

  int my_id, nprocs;
  MPI_Status status;
  MPI_Comm new_comm;
  int dims[2] = {4, 3};
  int period[2] = {1, 0};
  int reorder = 1;
  int my_coords[2], id;
	
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);

  if (nprocs != 12)	{
    printf("Please run the program with 12 MPI processes.\n");
    MPI_Abort(MPI_COMM_WORLD, 1);
  }
  
  MPI_Cart_create(MPI_COMM_WORLD, 2, dims, period, reorder, &new_comm);

  if (my_id == 5)	{
    MPI_Cart_coords(new_comm, my_id, 2, my_coords);
    printf("Process-%d coordinates are (%d, %d).\n", my_id, my_coords[0], my_coords[1]);
  }

  if (my_id == 0)	{
    my_coords[0] = 3;
    my_coords[1] = 1;
    MPI_Cart_rank(new_comm, my_coords, &id); /* determines the rank of a given process in the communicator for a given cartesian topology */
    printf("The process with coordinates (%d, %d) has rank = %d.\n", my_coords[0], my_coords[1], id);
  }
      
  MPI_Finalize();	
  return 0;
}
