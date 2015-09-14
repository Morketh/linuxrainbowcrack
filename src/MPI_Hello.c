#include "mpi.h"
#include <stdio.h>
int main(int argc ,char *argv[])
{
	int myrank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	fprintf(stdout, "Hello World, I am process  %d\n", myrank);
	MPI_Finalize();
	return 0;
}

