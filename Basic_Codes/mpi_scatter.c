// Simple scatter demonstration program in MPI
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	int i, my_id, nprocs;
	int *send_buff, recv_buff;
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	
	if (my_id == 0)	{
		send_buff = (int *)malloc(nprocs * sizeof(int));	// Allocate memory for send_buff in the root process
		for(i = 0; i < nprocs; i++)	
			send_buff[i] = 50 + i * 3;
	}
	
	MPI_Scatter(send_buff, 1, MPI_INT, &recv_buff, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (my_id == 0)
		printf("Values received by each processes are:\n");
	MPI_Barrier(MPI_COMM_WORLD);
	
	printf("%d, %d\n", my_id, recv_buff);
	
	if (my_id == 0)
		free(send_buff);
	
	MPI_Finalize();
	
	return 0;
}
