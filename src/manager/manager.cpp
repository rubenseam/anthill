# include <stdio.h>
# include <stdlib.h>
# include <mpi.h>
// # include "manager.hpp"


int main(int argc, char *argv[]) {

	int SIZE = 10;
    double a[SIZE][SIZE], b[SIZE], c[SIZE];
    int i, j, row, numworkers;
    MPI_Status status;
    MPI_Comm workercomm;

	MPI::Init(argc, argv);
    if (argc != 2)
        printf("usage: %s <number of workers>\n", argv[0]);
    else
        numworkers = atoi(argv[1]);
	printf(">>> %d WORKERS!\n", numworkers);

    MPI_Comm_spawn("../worker/worker", MPI_ARGV_NULL, numworkers,
               MPI_INFO_NULL,
               0, MPI_COMM_SELF, &workercomm, MPI_ERRCODES_IGNORE );

    /* initialize a and b */
    
	/* send b to each worker */
    
	MPI_Bcast( b, SIZE, MPI_DOUBLE, MPI_ROOT, workercomm );
    
	/* then normal manager code as before*/

	MPI::Finalize();
    return 0;

}
