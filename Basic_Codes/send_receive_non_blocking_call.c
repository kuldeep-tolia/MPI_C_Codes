#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[])
{
	MPI_Request request;
	MPI_Init(&argc, &argv);
	
	int size, rank, count = 1000000;
	
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int num1[count], num2[count];
	
	if (rank == 0) {
		for(int i = 0; i < count; i++)	{
			num1[i] = i;
		}
	}
	
	else if (rank == 1)	{
		for(int i = 0; i < count; i++)	{
			num1[i] = count - i - 1;
		}
	}
	
	MPI_Barrier(MPI_COMM_WORLD);
	
	if (rank == 0)	{
		MPI_Isend(num1, count, MPI_INT, 1, 10, MPI_COMM_WORLD, &request);
		MPI_Irecv(num2, count, MPI_INT, 1, 11, MPI_COMM_WORLD, &request);
	}
	
	else if (rank == 1)	{
		MPI_Isend(num1, count, MPI_INT, 0, 11, MPI_COMM_WORLD, &request);
		MPI_Irecv(num2, count, MPI_INT, 0, 10, MPI_COMM_WORLD, &request);
	}
	
	if (rank == 0) {
		printf("process-0 has received from process-1.\n");
	}
	else	{
		printf("process-1 has received from process-0.\n");
	}
	
	MPI_Finalize();
	
	return 0;
}
