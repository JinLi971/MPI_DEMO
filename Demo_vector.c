/* File: Demo_vector.c
 * This is an example for MPI_Type_vector
 * Shows how to use MPI_Type_vector to send noncontiguous blocks of data
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
 *  output
         numx=5  extent=5 stride=1
         got 1 elements of type MY_TYPE
         which contained 5 elements of type MPI_DOUBLE
         0 0
         5 5
         10 10
         15 15
         20 20
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#define SIZE 25
int main(int argc,char *argv[])
{
    int myid, numprocs,mpi_err;
    double sendbuf[SIZE],recvbuf[SIZE];
    int i,bonk1,bonk2,count,stride,blocklength;
    MPI_Datatype MPI_LEFT_RITE;
    MPI_Status status;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);

    stride=5;
    count=(SIZE+1)/stride;
    blocklength=2;
    if(myid == 1){
        printf("count=%d  blocklength=%d stride=%d\n",count,blocklength,stride);
    }
    mpi_err=MPI_Type_vector(count,blocklength,stride,MPI_DOUBLE,&MPI_LEFT_RITE);
    mpi_err=MPI_Type_commit(&MPI_LEFT_RITE);
    if(myid == 0){
        printf("send buf = ");
        for (i=0;i<SIZE;i++){
            sendbuf[i]=i;
            printf("%d ", i);
        }
        printf("\n");
        MPI_Send(sendbuf,1,MPI_LEFT_RITE,1,100,MPI_COMM_WORLD);
    }
    if(myid == 1){
        for (i=0;i<SIZE;i++)
            recvbuf[i]=-1;
        MPI_Recv(recvbuf,1,MPI_LEFT_RITE,0,100,MPI_COMM_WORLD,&status);
    }
    if(myid == 1){
        MPI_Get_count(&status,MPI_LEFT_RITE,&bonk1);
        MPI_Get_elements(&status,MPI_DOUBLE,&bonk2);
        printf("got %d elements of type MY_TYPE\n",bonk1);
        printf("which contained %d elements of type MPI_DOUBLE\n",bonk2);
        for (i=0;i<SIZE;i++)
            if(recvbuf[i] != -1)
                printf(" %g\n",recvbuf[i]);
    }
    MPI_Finalize();
}
