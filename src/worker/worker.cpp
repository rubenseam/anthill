# include <mpi.h>


int main(int argc, char *argv[]) {

	int SIZE = 10;
    int numprocs, myrank;
    double b[SIZE], c[SIZE];
    int i, row;
    double dotp;
    MPI_Status status;
    MPI_Comm parentcomm;

    MPI_Init( &argc, &argv );
    MPI_Comm_size( MPI_COMM_WORLD, &numprocs );
    MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

	printf("%d IS ALIVE!\n", myrank);
    MPI_Comm_get_parent( &parentcomm );

    MPI_Bcast( b, SIZE, MPI_DOUBLE, 0, parentcomm );

    /* same as worker code from original matrix-vector multiply */

    MPI_Comm_free( &parentcomm );
	MPI::Finalize();
    return 0;

}
