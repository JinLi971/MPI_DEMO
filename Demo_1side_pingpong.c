/* File: Demo_1side_pingpong.c
 * This is an pingpoing example using post start complete for syn
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
*/

#include <time.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv )
 {
  if (argc != 3) {
    printf("Usage: Demopp message_size message_number\n");
    return -1;
  }
  int numprocs, myid, j, i;
  int msize = atoi(argv[1]);
  int mnum  = atoi(argv[2]);

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  MPI_Comm_rank(MPI_COMM_WORLD, &myid);
  MPI_Win win;

  if (myid == 0)
      printf("One-sided ping-pong using post-start-complete for syncs\n");
  double *send;
  double *recv;
  send = (double *) calloc(msize, sizeof (double));
  recv = (double *) calloc(msize, sizeof (double));

  //Initialize the send vector to some random numbers
  srand(time(NULL));
  for (j=0; j < msize; j++) {
    send[j] = myid + (double)rand()/(double)(RAND_MAX-1);
    recv[j] = 0;
  }

  int soi = sizeof(int);
  MPI_Win_create(send, soi*msize, soi,
      MPI_INFO_NULL, MPI_COMM_WORLD, &win);
  //Sneaky way to set the target rank
  int target_rank = 1 - myid;

  //Set up the group
  MPI_Group comm_group, group;
  MPI_Comm_group(MPI_COMM_WORLD, &comm_group);
  MPI_Group_incl(comm_group, 1, &target_rank, &group);

  double start, stop, diff;
  start = MPI_Wtime();
  printf("Rank %d RECV should start out as zero: %8.8f\n",
         myid, recv[0]);
  for (i=0; i < mnum; i++) {
    MPI_Win_post(group, 0, win);
    //Start access epoch
    MPI_Win_start(group, 0, win);
    MPI_Get(recv, msize, MPI_DOUBLE, target_rank,
               0, msize, MPI_DOUBLE, win);
    MPI_Win_complete(win);
    MPI_Win_wait(win);

  }
  stop = MPI_Wtime();
  diff = stop - start;
  printf("Rank %d RECV should no longer be zero: %8.8f\n",
         myid, recv[0]);
  printf("Rank %d, %d %d-double transactions took %8.8fs\n",
         myid, mnum, msize, diff);
  MPI_Group_free(&group);
  MPI_Win_free(&win);
  MPI_Finalize();
  return 0;
 }

/*
-bash-4.1$ mpicc -o Demopp Demo_1side_pingpong.c
-bash-4.1$ mpirun -n 2 Demopp 10000 100
One-sided ping-pong using post-start-complete for syncs
Rank 0 RECV should start out as zero: 0.00000000
Rank 1 RECV should start out as zero: 0.00000000
Rank 0 RECV should no longer be zero: 1.69493750
Rank 0, 100 10000-double transactions took 0.00702286s
Rank 1 RECV should no longer be zero: 0.69493750
Rank 1, 100 10000-double transactions took 0.00701094s
*/

