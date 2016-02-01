/* File: Demo_isend.c
 * This is an example for MPI_Isend and MPI_Irecv
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int main(int argc,char *argv[])
{
    int myid, numprocs;
    int tag,source,destination,count;
    int buffer;
    MPI_Status status;
    MPI_Request req_send, req_recv;

    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    tag=1;
    source=0;
    destination=1;
    count=1;
    req_send=MPI_REQUEST_NULL;
    req_recv = MPI_REQUEST_NULL;
    if(myid == source){
      buffer=123456;
      MPI_Isend(&buffer,count,MPI_INT,destination,tag,MPI_COMM_WORLD,&req_send);
    }
    if(myid == destination){
        MPI_Irecv(&buffer,count,MPI_INT,source,tag,MPI_COMM_WORLD,&req_recv);
    }

    MPI_Wait(&req_send,&status);
    MPI_Wait(&req_recv,&status);
    if(myid == source){
      printf("processor %d  sent %d\n",myid,buffer);
    }
    if(myid == destination){
      printf("processor %d  got %d\n",myid,buffer);
    }
    MPI_Finalize();
}
/*
-bash-4.1$ mpicc -o DemoIsend Demo_isend.c
-bash-4.1$ mpirun -n 2 DemoIsend
processor 0  sent 123456
processor 1  got 123456
*/

