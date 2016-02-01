/*
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * File: Demo2dPart.c   examples to use MPI_Barrier MPI_Type_vector MPI_Scatterv MPI_Irecv MPI_Isend MPI_Wait
 * Date: Jan, 2015, last update: Jan 2016
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COLS  10
#define ROWS  12

int main(int argc, char **argv) {

    MPI_Init(&argc, &argv);
    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    const int NPROWS=4;  /* number of rows in partition */
    const int NPCOLS=2;  /* number of cols in patition */
    const int BLOCKROWS = ROWS/NPROWS;  /* number of rows in _block_ */
    const int BLOCKCOLS = COLS/NPCOLS; /* number of cols in _block_ */
    double *sendbuf = NULL;
    double *recvbuf =(double*) malloc(sizeof(double)*(BLOCKROWS*BLOCKCOLS));

    //reset recv buffer = 0
    for (int ii=0; ii<BLOCKROWS*BLOCKCOLS; ii++)
        recvbuf[ii] = 0;


    if (rank == 0) {
        printf("global matrix is %d*%d, and sub matrix is %d*%d\n", ROWS,COLS,BLOCKROWS   ,BLOCKCOLS);
        printf("grid is %d by %d \n", NPROWS,NPCOLS);
        sendbuf =  (double*) malloc(ROWS*COLS*sizeof(double));
        //set send buffer to a random number
        for (int ii=0; ii<ROWS*COLS; ii++) {
            sendbuf[ii] = drand48();
        }
    }

    MPI_Datatype Pdatatype;
    MPI_Datatype Wdatatype;
    MPI_Type_vector(BLOCKROWS, BLOCKCOLS, COLS, MPI_DOUBLE, &Wdatatype);
    MPI_Type_create_resized( Wdatatype, 0, sizeof(double), &Pdatatype);
    MPI_Type_commit(&Pdatatype);
    int disps[NPROWS*NPCOLS];
    int counts[NPROWS*NPCOLS];
    for (int ii=0; ii<NPROWS; ii++) {
        for (int jj=0; jj<NPCOLS; jj++) {
            disps[ii*NPCOLS+jj] = ii*COLS*BLOCKROWS+jj*BLOCKCOLS;
            counts [ii*NPCOLS+jj] = 1;
        }
    }

    MPI_Scatterv(sendbuf, counts, disps, Pdatatype, recvbuf, BLOCKROWS*BLOCKCOLS, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    /* each proc prints it's "b" out, in order */
    for (int proc=0; proc<p; proc++) {
        if (proc == rank) {
            if (rank == 0) {
                printf("Global matrix: \n");
                for (int ii=0; ii<ROWS; ii++) {
                    for (int jj=0; jj<COLS; jj++) {
                        printf("%.3f ",sendbuf[ii*COLS+jj]);
                    }
                    printf("\n");
                }
            }
            printf("Local Matrix: at rank %d\n",rank);
            for (int ii=0; ii<BLOCKROWS; ii++) {
                for (int jj=0; jj<BLOCKCOLS; jj++) {
                    printf("%.3f ",recvbuf[ii*BLOCKCOLS+jj]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    //for shifting Isend and Irecv
    double *recvbuf2 =(double*) malloc(sizeof(double)*(BLOCKROWS*BLOCKCOLS));
    int right = (rank + 1) % p;
    int left = rank - 1;
    if (left < 0)
        left = p - 1;
    MPI_Request request, request2;
    MPI_Status status;

    MPI_Irecv(recvbuf2, BLOCKROWS*BLOCKCOLS, MPI_DOUBLE , left, 123, MPI_COMM_WORLD, &request);
    MPI_Isend(recvbuf,  BLOCKROWS*BLOCKCOLS, MPI_DOUBLE , right, 123, MPI_COMM_WORLD, &request2);
    MPI_Wait(&request, &status);

    for (int proc=0; proc<p; proc++) {
        if (proc == rank) {
            printf("Rank = %d\n", rank);
            printf("Local Matrix:\n");
            for (int ii=0; ii<BLOCKROWS; ii++) {
                for (int jj=0; jj<BLOCKCOLS; jj++) {
                    printf("%.3f ",recvbuf2[ii*BLOCKCOLS+jj]);
                }
                printf("\n");
            }
            printf("\n");
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    double** sendbufd =NULL;
    double ** recvbufd = (double **) malloc( sizeof(double*) * BLOCKROWS);
    for (int i = 0; i<BLOCKROWS; i++)
        recvbufd[i] = (double*) malloc(sizeof(double) * BLOCKCOLS);

    if(rank == 0 ){
        sendbufd = (double**) malloc(sizeof(double*) * ROWS   );
        for (int i = 0; i< ROWS; i++)
            sendbufd [i] = (double*) malloc (sizeof(double) * COLS   );
        for(int i =0; i<ROWS ;i++){
            for (int j = 0; j< COLS; j++){
                sendbufd[i][j] = drand48();
            }
        }
    }

    MPI_Finalize();
    return 0;
}
/* Results:
-bash-4.1$ mpirun -n 8 Demo2dPart
global matrix is 12*10, and sub matrix is 3*5
grid is 4 by 2
Global matrix:
0.000 0.001 0.042 0.177 0.365 0.091 0.092 0.487 0.527 0.454
0.233 0.831 0.932 0.568 0.556 0.051 0.767 0.019 0.252 0.298
0.876 0.532 0.920 0.515 0.810 0.188 0.886 0.571 0.077 0.815
0.985 0.118 0.894 0.784 0.101 0.253 0.020 0.378 0.679 0.681
0.753 0.006 0.624 0.126 0.618 0.771 0.187 0.497 0.510 0.316
0.702 0.367 0.876 0.371 0.525 0.428 0.543 0.668 0.171 0.831
0.862 0.769 0.053 0.967 0.334 0.082 0.270 0.074 0.676 0.810
0.464 0.569 0.512 0.587 0.739 0.872 0.578 0.965 0.334 0.393
0.177 0.112 0.853 0.811 0.955 0.904 0.139 0.686 0.127 0.004
0.001 0.193 0.587 0.375 0.027 0.465 0.785 0.442 0.288 0.853
0.526 0.728 0.266 0.783 0.468 0.197 0.447 0.728 0.966 0.757
0.466 0.869 0.134 0.846 0.018 0.941 0.670 0.800 0.143 0.610
Local Matrix: at rank 0
0.000 0.001 0.042 0.177 0.365
0.233 0.831 0.932 0.568 0.556
0.876 0.532 0.920 0.515 0.810

Local Matrix: at rank 1
0.091 0.092 0.487 0.527 0.454
0.051 0.767 0.019 0.252 0.298
0.188 0.886 0.571 0.077 0.815

Local Matrix: at rank 2
0.985 0.118 0.894 0.784 0.101
0.753 0.006 0.624 0.126 0.618
0.702 0.367 0.876 0.371 0.525

Local Matrix: at rank 3
0.253 0.020 0.378 0.679 0.681
0.771 0.187 0.497 0.510 0.316
0.428 0.543 0.668 0.171 0.831

Local Matrix: at rank 4
0.862 0.769 0.053 0.967 0.334
0.464 0.569 0.512 0.587 0.739
0.177 0.112 0.853 0.811 0.955

Local Matrix: at rank 5
0.082 0.270 0.074 0.676 0.810
0.872 0.578 0.965 0.334 0.393
0.904 0.139 0.686 0.127 0.004

Local Matrix: at rank 6
0.001 0.193 0.587 0.375 0.027
0.526 0.728 0.266 0.783 0.468
0.466 0.869 0.134 0.846 0.018

Local Matrix: at rank 7
0.465 0.785 0.442 0.288 0.853
0.197 0.447 0.728 0.966 0.757
0.941 0.670 0.800 0.143 0.610

Rank = 0
Local Matrix:
0.465 0.785 0.442 0.288 0.853
0.197 0.447 0.728 0.966 0.757
0.941 0.670 0.800 0.143 0.610

Rank = 1
Local Matrix:
0.000 0.001 0.042 0.177 0.365
0.233 0.831 0.932 0.568 0.556
0.876 0.532 0.920 0.515 0.810

Rank = 2
Local Matrix:
0.091 0.092 0.487 0.527 0.454
0.051 0.767 0.019 0.252 0.298
0.188 0.886 0.571 0.077 0.815

Rank = 3
Local Matrix:
0.985 0.118 0.894 0.784 0.101
0.753 0.006 0.624 0.126 0.618
0.702 0.367 0.876 0.371 0.525

Rank = 4
Local Matrix:
0.253 0.020 0.378 0.679 0.681
0.771 0.187 0.497 0.510 0.316
0.428 0.543 0.668 0.171 0.831

Rank = 5
Local Matrix:
0.862 0.769 0.053 0.967 0.334
0.464 0.569 0.512 0.587 0.739
0.177 0.112 0.853 0.811 0.955

Rank = 6
Local Matrix:
0.082 0.270 0.074 0.676 0.810
0.872 0.578 0.965 0.334 0.393
0.904 0.139 0.686 0.127 0.004

Rank = 7
Local Matrix:
0.001 0.193 0.587 0.375 0.027
0.526 0.728 0.266 0.783 0.468
0.466 0.869 0.134 0.846 0.018
*
*/
