/* File: hello_mpi.c
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
*/
#include <stdio.h>
#include <mpi.h>

int main (int argc, char* argv[]){
    int rank, size;
    MPI_Init (&argc, &argv);	/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);	/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);	/* get number of processes */
    printf( "Hello MPI from process %d of %d\n", rank, size );
    MPI_Finalize();
    return 0;
}
