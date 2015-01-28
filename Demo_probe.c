/* File: Demo_probe.c
 * This is an example for MPI_Probe and MPI_Get_count
 * shows how to find the size of incomming message
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
int main(int argc,char *argv[])
{
    int myid, numprocs;
    MPI_Status status;
    int mytag,ierr,icount,*recv,j;
    int send[3] = {1,2,3};
    MPI_Init(&argc,&argv);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    mytag=1;
    if(myid == 0) {
        icount=3;
        ierr=MPI_Send(&send[0],icount,MPI_INT,1,mytag,MPI_COMM_WORLD);
    }
    if(myid == 1){
        ierr=MPI_Probe(0,mytag,MPI_COMM_WORLD,&status);
        ierr=MPI_Get_count(&status,MPI_INT,&icount);
        recv = (int*)malloc(icount*sizeof(int));
        printf("getting %d values \n",icount);
        ierr = MPI_Recv(recv,icount,MPI_INT,0,mytag,MPI_COMM_WORLD,&status);
        printf("recv message = ");
        for(j=0;j<icount;j++)
            printf("%d ",recv[j]);
        printf("\n");
    }
    MPI_Finalize();
}
