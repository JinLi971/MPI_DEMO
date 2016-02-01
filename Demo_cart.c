/* File: Demo_cat.c
 * This is an example for MPI_Cart_create and MPI_Cart_coords,MPI_Cart_rank
 * shows how to find the size of incomming message
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int nrow, mcol, root, myid, ndim, p, rank;
    int dims[2], coords[2], cyclic[2], reorder;
    MPI_Comm comm2D;
    /* Starts MPI processes ... */
    MPI_Init(&argc, &argv);               /* starts MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &myid);  /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &p);    /* get number of processes */

    nrow = 3;
    mcol = 2;
    ndim = 2;
    root = 0;
    cyclic[0] = 0;
    cyclic[1] = 0;
    reorder = 1; // change reorder = 0; try several times to see differences.

    if(myid == root) {
        printf("\n");
        printf("There are six (6) processes\n");
        printf("use all 6 to create 3x2 cartesian topology\n");
        printf("    Cart. Coords.     Cart\n");
        printf("   i   j   rank   myid\n");
    }
    MPI_Barrier(MPI_COMM_WORLD);
    /* first, create 3x2 cartesian topology for processes */
    dims[0] = nrow;      /* rows */
    dims[1] = mcol;      /* columns */
    /*  */
    MPI_Cart_create(MPI_COMM_WORLD, ndim, dims, cyclic, reorder, &comm2D);
    /*  */

    if (comm2D != MPI_COMM_NULL) {
        MPI_Cart_coords(comm2D, myid, ndim, coords);
        MPI_Cart_rank(comm2D, coords, &rank);
        printf("%4d %4d %4d %4d\n",coords[0], coords[1], rank, myid);
    }
    MPI_Finalize();
}


/*-bash-4.1$ mpicc -o Democart Demo_cart.c
 * -bash-4.1$ mpirun -n 6 Democart

There are six (6) processes
use all 6 to create 3x2 cartesian topology
    Cart. Coords.     Cart
   i   j   rank   myid
   0    0    0    0
   0    1    1    1
   1    0    2    2
   1    1    3    3
   2    0    4    4
   2    1    5    5
*/

