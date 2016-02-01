/* File: Demo_put.c
 * This is an example of MPI one-side communication, MPI_Put
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
 * * If everything works well, then no output
*/
#include "mpi.h"
#include "stdio.h"

/* tests put and get with post/start/complete/wait on 2 processes */

#define SIZE1 100
#define SIZE2 200

int main(int argc, char *argv[])
{
    int rank, destrank, nprocs, *A, *B, i;
    MPI_Group comm_group, group;
    MPI_Win win;
    int errs = 0;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&nprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    if (nprocs != 2) {
        printf("Run this program with 2 processes\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    i = MPI_Alloc_mem(SIZE2 * sizeof(int), MPI_INFO_NULL, &A);
    //error check
    if (i>0) {
        printf("Can't allocate memory in test program\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    i = MPI_Alloc_mem(SIZE2 * sizeof(int), MPI_INFO_NULL, &B);
    //error check
    if (i>0) {
        printf("Can't allocate memory in test program\n");
        fflush(stdout);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Comm_group(MPI_COMM_WORLD, &comm_group);

    if (rank == 0) {
        for (i=0; i<SIZE2; i++)
            A[i] = B[i] = i;
        MPI_Win_create(NULL, 0, 1, MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        destrank = 1;
        MPI_Group_incl(comm_group, 1, &destrank, &group);
        MPI_Win_start(group, 0, win);
        for (i=0; i<SIZE1; i++)
            MPI_Put(A+i, 1, MPI_INT, 1, i, 1, MPI_INT, win);
        for (i=0; i<SIZE1; i++)
            MPI_Get(B+i, 1, MPI_INT, 1, SIZE1+i, 1, MPI_INT, win);

        MPI_Win_complete(win);

        for (i=0; i<SIZE1; i++)
            if (B[i] != (-4)*(i+SIZE1)) {
                printf("Get Error: B[i] is %d, should be %d\n", B[i], (-4)*(i+SIZE1));
                fflush(stdout);
                errs++;
            }
    }
    else { /* rank=1 */
        for (i=0; i<SIZE2; i++)
            B[i] = (-4)*i;
        MPI_Win_create(B, SIZE2*sizeof(int), sizeof(int), MPI_INFO_NULL, MPI_COMM_WORLD, &win);
        destrank = 0;
        MPI_Group_incl(comm_group, 1, &destrank, &group);
        MPI_Win_post(group, 0, win);
        MPI_Win_wait(win);

        for (i=0; i<SIZE1; i++) {
            if (B[i] != i) {
                printf("Put Error: B[i] is %d, should be %d\n", B[i], i);
                fflush(stdout);
                errs++;
            }
        }
    }

    MPI_Group_free(&group);
    MPI_Group_free(&comm_group);
    MPI_Win_free(&win);
    MPI_Free_mem(A);
    MPI_Free_mem(B);

    MPI_Finalize();
    return errs;
}
/*
-bash-4.1$ mpicc -o Demoput ../Demo_put.c
-bash-4.1$ mpirun -n 2 Demoput
 */
