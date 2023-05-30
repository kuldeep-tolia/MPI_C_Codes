// Simple broadcast demonstration program in MPI
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	int my_id, nprocs;
	int buff = 0;
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	
	if (my_id == 0)
		buff = 123;
	
	printf("Before broadcasting, process-id = %d, buff value = %d\n", my_id, buff);
	MPI_Barrier(MPI_COMM_WORLD);
	
	MPI_Bcast(&buff, 1, MPI_INT, 0, MPI_COMM_WORLD);
		
	printf("Ater broadcasting, process-id = %d, buff value = %d\n", my_id, buff);
	
	MPI_Finalize();
	
	return 0;
}
