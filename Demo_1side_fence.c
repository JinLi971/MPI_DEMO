/* File: Demo_1side_fence.c
 * This is an pingpoing example using post start complete for syn
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
 * This does a transpose and accumulate operations. Run on 2 processes
*/

#include "mpi.h"
#include "stdio.h"

#define NROWS 100
#define NCOLS 100

int main(int argc, char *argv[])
{
    int rank, nprocs, A[NROWS][NCOLS], i, j;
    MPI_Win win;
    MPI_Datatype column, xpose;
    int errs = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (nprocs != 2)
    {
        printf("Run this program with 2 processes\n");fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
    }


    for (i=0; i<NROWS; i++)
        for (j=0; j<NCOLS; j++)
            A[i][j] = i*NCOLS + j;
    if (rank == 0)
    {
        /* create datatype for one column */
        MPI_Type_vector(NROWS, 1, NCOLS, MPI_INT, &column);
        /* create datatype for matrix in column-major order */
        MPI_Type_hvector(NCOLS, 1, sizeof(int), column, &xpose);
        MPI_Type_commit(&xpose);
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        // Fence for windows creation
        MPI_Win_fence(0, win);
        //target at processor 1
        MPI_Accumulate(A, NROWS*NCOLS, MPI_INT, 1, 0, 1, xpose, MPI_SUM, win);

        MPI_Type_free(&column);
        MPI_Type_free(&xpose);
        //fence for accually do the computation
        MPI_Win_fence(0, win);
    }
    else
    { /* rank = 1 */
        MPI_Win_create(A, NROWS*NCOLS*sizeof(int), sizeof(int), MPI_INFO_NULL,
                       MPI_COMM_WORLD, &win);
        MPI_Win_fence(0, win); //for window creation
        MPI_Win_fence(0, win); // for computation

        for (j=0; j<NCOLS; j++)
        {
            for (i=0; i<NROWS; i++)
            {
                if (A[j][i] != i*NCOLS + j + j*NCOLS + i)
                {
                    if (errs < 50)
                    {
                        printf("Error: A[%d][%d]=%d should be %d\n", j, i,
                               A[j][i], i*NCOLS + j + j*NCOLS + i);
                        fflush(stdout);
                    }
                    errs++;
                }
            }
        }
        if (errs >= 50)
        {
            printf("Total number of errors: %d\n", errs);
            fflush(stdout);
        }
    }
    MPI_Win_free(&win);
    MPI_Finalize();
    return 0;
}
