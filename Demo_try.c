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

    const int NPROWS=4;  /* number of rows in _decomposition_ */
    const int NPCOLS=2;  /* number of cols in _decomposition_ */
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

    MPI_Finalize();
    return 0;
}
