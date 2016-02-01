/* File: Demo_index.c
 * This is an  example for MPI_Type_indexed and MPI_Type_contiguous
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
 * major functions used: MPI_Type_indexed MPI_Type_contiguous
*/
#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Datatype type, type2;
    int blocklen[3] = { 2, 3, 1 };
    int displacement[3] = { 0, 2, 7 };
    int buffer[27];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size < 2 || size>2)
    {
        printf("Please run with 2 processes.\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Type_contiguous(3, MPI_INT, &type2);
    MPI_Type_commit(&type2);
    MPI_Type_indexed(3, blocklen, displacement, type2, &type);
    MPI_Type_commit(&type);

    if (rank == 0)
    {
        for (i=0; i<27; i++)
            buffer[i] = i;
        // try this also
        //        MPI_Rsend(&buffer[2], 1, type, 1, 123, MPI_COMM_WORLD);
        MPI_Rsend(buffer, 1, type, 1, 123, MPI_COMM_WORLD);
    }

    if (rank == 1)
    {
        for (i=0; i<27; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);
        for (i=0; i<27; i++)
            printf("buffer[%d] = %d\n", i, buffer[i]);
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
/*
-bash-4.1$ mpicc -o DemoIndex Demo_index.c
-bash-4.1$ mpirun -n 2 DemoIndex
buffer[0] = 0
buffer[1] = 1
buffer[2] = 2
buffer[3] = 3
buffer[4] = 4
buffer[5] = 5
buffer[6] = 6
buffer[7] = 7
buffer[8] = 8
buffer[9] = 9
buffer[10] = 10
buffer[11] = 11
buffer[12] = 12
buffer[13] = 13
buffer[14] = 14
buffer[15] = -1
buffer[16] = -1
buffer[17] = -1
buffer[18] = -1
buffer[19] = -1
buffer[20] = -1
buffer[21] = 21
buffer[22] = 22
buffer[23] = 23
buffer[24] = -1
buffer[25] = -1
buffer[26] = -1
*/
