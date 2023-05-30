// An example for an SPMD program by the use of if-else conditional statements
#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
	int i, my_id, size, tag = 100;
	char send_buff[50], recv_buff[50];
	MPI_Status status;
	
	MPI_Init (&argc, &argv);		// initialize MPI, can also be written as MPI_Init(NULL, NULL);
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);	// tells about #processes in the communicator named MPI_COMM_WORLD --> default communicator
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);	// tells about rank of a particular process in the communicator
	
	if (my_id != 0)	{
		sprintf(send_buff, "Hello from process id = %d", my_id);
		MPI_Send(send_buff, 50, MPI_CHAR, 0, tag, MPI_COMM_WORLD);
	}
	else        {
		for(i = 1; i < size; i++)	{
			MPI_Recv(recv_buff, 50, MPI_CHAR, i, tag, MPI_COMM_WORLD, &status);
			printf("%s\n", recv_buff);
		}
		
		sprintf(send_buff, "Hello from process id = %d", my_id);
		printf("%s\n", send_buff);
	} 
	
	MPI_Finalize();		// stop MPI
	
	return 0;
}
