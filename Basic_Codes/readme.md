-> This section discusses about basic MPI calls like send-receive (blocking/non-blocking), reduction, broadcast, scatter(v), gather(v), allreduce, allgather, cart-create, cart-rank, etc.  
-> The computer programs only demonstrates the behaviour of a particular MPI call. To check the syntax/operation of a particular MPI clause/construct, you may refer to the following websites which provides description of each clause/construct:
- https://www.open-mpi.org/doc/v3.1/  
- https://rookiehpc.org/mpi/docs/index.html  

-> I have used mpicc wrapper to compile and link my MPI programs.  
-> Compiling and running a C program:
- $ mpicc file_name.c -lm -o ./output_name.out
- $ mpirun -np <num_process> ./output_name.out
