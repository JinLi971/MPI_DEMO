/* File: Demo_spawn.c
 * This is an example for MPI_Comm_spawn & MPI_Intercomm_merge
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
 */
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
int main(int argc, char **argv)
{
    int initialized;
    MPI_Comm boss_comm;

    MPI_Initialized(&initialized);
    if (!initialized) {
        MPI_Init(&argc,&argv);

        MPI_Comm_get_parent(&boss_comm);
        if (boss_comm != MPI_COMM_NULL) {
            // inside the spawned tasks
            int w_size,my_rank;
            MPI_Comm comm,intercom;

            MPI_Comm_size(MPI_COMM_WORLD,&w_size);
            MPI_Comm_rank(MPI_COMM_WORLD,&my_rank);
            printf("Slave task started, world size=%d my rank=%d\n",w_size,my_rank);

            printf("Slave task, merging the communicator...\n");
            MPI_Intercomm_merge(boss_comm,1,&comm);

            MPI_Comm_size(comm,&w_size);
            MPI_Comm_rank(comm,&my_rank);
            printf("Slave task after the merge, world size=%d my rank=%d\n",w_size,my_rank);


        } else {
            // the first task
            char** args = malloc(sizeof(char*));
            args[0]=NULL;
            MPI_Comm intercomm_1,intercomm_2,comm_1,comm_2,full_comm1,full_comm2;
            MPI_Comm intercomm_full;

            printf("Master task: first spawn\n");
            MPI_Comm_dup(MPI_COMM_SELF,&comm_1);
            MPI_Comm_spawn("Demospawn",args,2,MPI_INFO_NULL,0,comm_1,&intercomm_1,(int*)MPI_ERRCODES_IGNORE);

            printf("Master task: second spawn\n");
            MPI_Comm_dup(MPI_COMM_SELF,&comm_2);
            MPI_Comm_spawn("Demospawn",args,2,MPI_INFO_NULL,0,comm_2,&intercomm_2,(int*)MPI_ERRCODES_IGNORE);

            printf("Master task: the two groups have been spawned\n");
            printf("Master task: merging the first communicator...\n");
            MPI_Intercomm_merge(intercomm_1,0,&full_comm1);

            printf("Master task: merging the second communicator...\n");
            MPI_Intercomm_merge(intercomm_2,0,&full_comm2);
        }

    }
  MPI_Finalize();
}
