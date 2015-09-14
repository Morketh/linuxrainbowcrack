#include "mpi.h"
#include <stdio.h>
int main(int argc ,char *argv[])
{
	int myrank, nprocs, merror;
	merror = MPI_Init(&argc, &argv);
	if (merror != 0) {
		printf("Error initializing MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD,merror);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	fprintf(stdout, "Hello World, I am process (rank) %d\n", myrank);
	printf("Number of processors or tasks = %d My rank= %d\n", nprocs,myrank);
	
	
	MPI_Finalize();
	return 0;
}