Problem Description:  

-> This is a program to integrate a given function numerically using the Trapezoidal rule. Consider evaluation of the following integral:  
$$I = \int_{0}^{\pi} 1 + sin(x) \hspace{1mm} dx$$  
-> The exact value of the integration $I = 5.141592654$.  
-> The computer program can is parallelized using the reduction operation.  
-> However, sometimes the input quantities need not be hard-coded and can be read from user input by a particular process. These input values has to be communicated to other values.  
-> For efficient MPI communication, it is a good idea to use MPI derived datatypes to communicate multiple values in a single MPI call.  
-> Thus, 2 versions of the program demonstrates how to parallelize the trapezoidal rule for numerical integration with different MPI communication approaches. 
