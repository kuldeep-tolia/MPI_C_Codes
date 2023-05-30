// MPI parallelized version of Simpson rule using MPI derived data types
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <mpi.h>

double func(double x)	{
  return (sin(x) / (2.0 * pow(x, 3)));	// given function to integrate
}

double simpson_rule(double x_s, double x_e, int n, double h)	{
	
  double partial_sum, x;
  int i;
  partial_sum = (func(x_s) + func(x_e));
  
  for(i = 1; i <= n-1; i++)	{
    x = x_s + i * h;
    if ((i%2) == 0)	{
      partial_sum += 2.0 * func(x);
    }
    else	{
      partial_sum += 4.0 * func(x);
    }
  }

  partial_sum *= h / 3.0;
  return partial_sum;
}

void create_new_mpi_type(double* a_p, double* b_p, int* n_p, MPI_Datatype* new_mpi_type_p)	{	// subroutine to create a new mpi datatype
  int block_lengths[3] = {1, 1, 1};
  MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
  MPI_Aint a_addr, b_addr, n_addr;		// Aint means address-integer
  MPI_Aint displacements[3] = {0, 0, 0};
	
  MPI_Get_address(a_p, &a_addr);	// addresses will be corresponding to each process
  MPI_Get_address(b_p, &b_addr);
  MPI_Get_address(n_p, &n_addr);
	
  // offset wrt to first argument
  displacements[0] = a_addr - a_addr;	
  displacements[1] = b_addr - a_addr;	
  displacements[2] = n_addr - a_addr;	
	
  MPI_Type_create_struct(3, block_lengths, displacements, types, new_mpi_type_p);	// creating new MPI structure
  MPI_Type_commit(new_mpi_type_p);	// commit new MPI datatype for MPI's bookkeeping
}

void read_user_input(int my_id, int nprocs, double* a_p, double* b_p, int* n_p)	{
  MPI_Datatype new_mpi_type;

  create_new_mpi_type(a_p, b_p, n_p, &new_mpi_type);
  if(my_id == 0)	{
    printf("Enter values of a, b, n:\n");
    scanf("%lf %lf %d", a_p, b_p, n_p);
  }
	
  MPI_Bcast(a_p, 1, new_mpi_type, 0, MPI_COMM_WORLD);	// Broadcast values to all other processes in the communicator
  MPI_Type_free(&new_mpi_type);		// free the memory of new mpi datatype  
} 

int main(int argc, char *argv[])	{

  double a, b, integration_result, local_a, local_b, local_sum, h, exact_result;
  int n, local_n, my_id, nprocs;
  MPI_Status status;
	
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	
  read_user_input(my_id, nprocs, &a, &b, &n);
  integration_result = 0.0;
  exact_result = 0.198573;
  
  h = (b - a) / n;
  local_n = n / nprocs;		// nprocs evenly divides with the number of divisions
  local_a = a + my_id * local_n * h;
  local_b = local_a + local_n * h;
  local_sum = simpson_rule(local_a, local_b, local_n, h);
  MPI_Reduce(&local_sum, &integration_result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (my_id == 0)	{
  	printf("\nThe integration for the given function between limits %lf and %lf = %0.9f\n", a, b, integration_result);
	printf("Error associated between the numerically obtained value and the exact value = %0.9f\n", fabs(integration_result - exact_result));
  }
  
  MPI_Finalize();	
  return 0;
}
