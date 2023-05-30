// Simple gather demonstration program in MPI
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	int i, my_id, nprocs;
	int send_buff, *recv_buff;
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	
	if (my_id == 0)	{
		recv_buff = (int *)malloc(nprocs * sizeof(int));	// Allocate memory for recv_buff in the root process
	}
	
	send_buff = 50 + my_id * 3;
	
	MPI_Gather(&send_buff, 1, MPI_INT, recv_buff, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	if (my_id == 0)	{
		printf("Values received on root process are:\n");
		for(i = 0; i < nprocs; i++)
			printf("recv_buff[%d] = %d\n", i, recv_buff[i]);
		
		free(recv_buff);
	} 
	
	MPI_Finalize();
	
	return 0;
}
