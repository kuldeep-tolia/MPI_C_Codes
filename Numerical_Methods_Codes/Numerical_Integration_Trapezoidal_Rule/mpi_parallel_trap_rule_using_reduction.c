// MPI parallelized version of trapezoidal rule using reduction operation
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

#define PI 3.14159265358

double func(double x)	{
	return (1.0 + sin(x));		// given function to integrate
}

double trap_rule(double x_s, double x_e, int n, double h)	{
	
	double partial_sum, x;
	int i;
		
	partial_sum = (func(x_s) + func(x_e)) / 2.0;
	
	for(i = 1; i <= n-1; i++)	{
		x = x_s + i * h;
		partial_sum = partial_sum + func(x);
	}
	
	partial_sum = partial_sum * h;
	
	return partial_sum;
}

int main(int argc, char *argv[])	{

	double a, b, integration_result, local_a, local_b, local_sum, h;
	int n, local_n, my_id, nprocs, i;
	MPI_Status status;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	
	n = 1024;	// number of divisions for integration
	a = 0.0;	// integration lower limit
	b = PI;	// integration upper limit
	integration_result = 0.0;
	
	h = (b - a) / n;
	local_n = n / nprocs;			// nprocs evenly divides with the number of divisions 
	local_a = a + my_id * local_n * h;
	local_b = local_a + local_n * h;
	local_sum = trap_rule(local_a, local_b, local_n, h);
	
	MPI_Reduce(&local_sum, &integration_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
	if (my_id == 0)	{
		printf("\nThe integration for the given function between limits %lf and %lf = %lf.\n", a, b, integration_result);
	}
	
	MPI_Finalize();	

	return 0;
}
