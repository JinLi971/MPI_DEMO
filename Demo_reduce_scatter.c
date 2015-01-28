/* File: Demo_reduce_scatter.c
 * This is an example for MPI_Scatter and MPI_Reduce
 * shows how to find the size of incomming message
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int numnodes,myid,mpi_err;
#define ROOT 0

void init_it(int  *argc, char ***argv);

void init_it(int  *argc, char ***argv) {
    mpi_err = MPI_Init(argc,argv);
    mpi_err = MPI_Comm_size( MPI_COMM_WORLD, &numnodes );
    mpi_err = MPI_Comm_rank(MPI_COMM_WORLD, &myid);
}

int main(int argc,char *argv[]){
    int *recvbuf,*sendbuf;
    int count;
    int size,i,j,total,gtotal;

    init_it(&argc,&argv);
/* each processor will get count elements from the root */
    count=4;
    recvbuf=(int*)malloc(count*sizeof(int));
/* create the data to be sent on the root */
    if(myid == ROOT){
        size=count*numnodes;
        sendbuf=(int*)malloc(size*sizeof(int));
        printf("messges in root is ");
        for(i=0;i<size;i++)
            sendbuf[i]=i;
            printf("%d ", sendbuf[i]);
        }
        printf("\n");
/* send different data to each processor */
    mpi_err = MPI_Scatter(	sendbuf, count,   MPI_INT,
                            recvbuf,    count,   MPI_INT,
                            ROOT,
                            MPI_COMM_WORLD);
/* each processor does a local sum */
    total=0;
    for(i=0;i<count;i++){
        total=total+recvbuf[i];
        printf("recv message at process %d, and the %dth value is %d\n",myid,count, recvbuf[i]);
    }
    printf("myid= %d total= %d\n ",myid,total);
/* send the local sums back to the root */
    mpi_err = MPI_Reduce(&total,    &gtotal, 1,  MPI_INT,
                        MPI_SUM,
                        ROOT,
                        MPI_COMM_WORLD);
/* the root prints the global sum */
    if(myid == ROOT){
      printf("Global sum from all processors= %d \n ",gtotal);
    }
    mpi_err = MPI_Finalize();
}

