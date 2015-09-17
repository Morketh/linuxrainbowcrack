#include "Public.h"
#include "mpi.h"
#include <stdio.h>

int main(int argc ,char *argv[])
{
	int myrank, nprocs, merror;
	merror = MPI_Init(&argc, &argv);
	if (merror != 0) 
	{
		printf("Error initializing MPI program. Terminating.\n");
		MPI_Abort(MPI_COMM_WORLD,merror);
	}
	MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	if (myrank == 0)
	{ //only print logo on the first task
		Logo();
	}
	fprintf(stdout, "Hello World, I am process %d of %d running on node: %s\n", myrank, nprocs, GetNodeName());	
	MPI_Finalize();
	return 0;
}

