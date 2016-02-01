/* File: Demo_index_vector.c
 * This is an  example for MPI_Type_indexed and MPI_Type_vector
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2016
*/
#include "mpi.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    int rank, size, i;
    MPI_Datatype type,type2;
    int blocklen[3] = { 1,2,1 };
    int displacement[3] = { 0, 3,7 };
    int buffer[100];
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size < 2)
    {
        printf("Please run with 2 processes.\n");
        MPI_Finalize();
        return 1;
    }
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Type_vector(3,2,4,MPI_INT,&type2);
    MPI_Type_commit(&type2);
    MPI_Type_indexed(3, blocklen, displacement, type2, &type);
    MPI_Type_commit(&type);

    if (rank == 0)
    {
        for (i=0; i<100; i++)
            buffer[i] = i;
        // try this also
        //        MPI_Send(&buffer[2], 1, type, 1, 123, MPI_COMM_WORLD);
        MPI_Rsend(&buffer[0], 1, type, 1, 123, MPI_COMM_WORLD);
    }

    if (rank == 1)
    {
        for (i=0; i<100; i++)
            buffer[i] = -1;
        MPI_Recv(buffer, 1, type, 0, 123, MPI_COMM_WORLD, &status);
        for (i=0; i<100; i++){
            if (buffer[i]!=-1)
        
            printf("buffer[%d] = %d\n", i, buffer[i]);
        }
        fflush(stdout);
    }

    MPI_Finalize();
    return 0;
}
/*
-bash-4.1$ mpicc -o DemoIndexVector ../Demo_index_vector.c
-bash-4.1$ mpirun -n 2 DemoIndexVector
buffer[0] = 0
buffer[1] = 1
buffer[4] = 4
buffer[5] = 5
buffer[8] = 8
buffer[9] = 9
buffer[30] = 30
buffer[31] = 31
buffer[34] = 34
buffer[35] = 35
buffer[38] = 38
buffer[39] = 39
buffer[40] = 40
buffer[41] = 41
buffer[44] = 44
buffer[45] = 45
buffer[48] = 48
buffer[49] = 49
buffer[70] = 70
buffer[71] = 71
buffer[74] = 74
buffer[75] = 75
buffer[78] = 78
buffer[79] = 79
*/
