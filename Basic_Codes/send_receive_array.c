#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main (int argc, char* argv[])
{
	int size, rank;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	if (size != 2)	{
		printf("Use exactly 2 processes to run this code.\n");
		MPI_Abort(MPI_COMM_WORLD, 1);
	}
	
	int count = 10;
	int num[count], num1[count];
	
	if (rank == 0) {
		for(int i = 0; i < count; i++)	{
			num[i] = i;
		}
		
		MPI_Send(num, count, MPI_INT, 1, 11, MPI_COMM_WORLD);
		MPI_Recv(num1, count, MPI_INT, 1, 12, MPI_COMM_WORLD, &status);
		
		for(int i = 0; i < count; i++)	{
			printf("process-0 has received num[%d] = %d from process-1.\n", i, num1[i]);
		}
	}
	
	else if (rank == 1)	{
		MPI_Recv(num1, count, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);
		
		for(int i = 0; i < count; i++)	{
			printf("process-1 received num[%d] = %d from process-0.\n", i, num1[i]);
			num[i] = count - i - 1;
		}
		
		MPI_Send(num, count, MPI_INT, 0, 12, MPI_COMM_WORLD);
	}
	
	MPI_Finalize();	
}
