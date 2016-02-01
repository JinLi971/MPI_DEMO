/* File: Demo_get.c
 * This is an example for one-sided communication MPI_Get
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
 */
#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv)
{
    int rank, size, namelen, version, subversion, *a, *b, i;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    MPI_Win win;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Get_processor_name(processor_name, &namelen);
    MPI_Get_version(&version, &subversion);
    printf("Hello world! I’m rank %d of %d on %s running MPI %d.%d\n",
           rank, size, processor_name, version, subversion);

    MPI_Alloc_mem(sizeof(int)*size, MPI_INFO_NULL, &a);
    MPI_Alloc_mem(sizeof(int)*size, MPI_INFO_NULL, &b);
    MPI_Win_create(a, size, sizeof(int), MPI_INFO_NULL,
                   MPI_COMM_WORLD, &win);

    for (i = 0; i < size; i++)
        a[i] = rank * 100 + i;
    printf("Process %d has the following:", rank);
    for (i = 0; i < size; i++)
        printf(" %d", a[i]);
    printf("\n");
    MPI_Win_fence((MPI_MODE_NOPUT | MPI_MODE_NOPRECEDE), win);

    for (i = 0; i < size; i++)
        MPI_Get(&b[i], 1, MPI_INT, i, rank, 1, MPI_INT, win);

    MPI_Win_fence(MPI_MODE_NOSUCCEED, win);
    printf("Process %d obtained the following:", rank);
    for (i = 0; i < size; i++)
        printf( "%d ", b[i]);
    printf("\n");

    MPI_Win_free(&win);
    MPI_Free_mem(a);
    MPI_Free_mem(b);
    MPI_Finalize();
}

/*
-bash-4.1$ mpicc -o Demoget ../Demo_get.c
-bash-4.1$ mpirun -n 2 Demoget
Hello world! I’m rank 0 of 2 on login running MPI 3.0
Hello world! I’m rank 1 of 2 on login running MPI 3.0
Process 0 has the following: 0 1
Process 1 has the following: 100 101
Process 0 obtained the following:0 100
Process 1 obtained the following:1 101
 */
