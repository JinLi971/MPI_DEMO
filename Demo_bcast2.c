/* File: Demo_bcast2.c
 * This is an Broadcast example using a for loop of
 * MPI_Send/MPI_Recv and MPI_Bcast()
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>

// use a for loop to perform a the broadcast
void SR_bcast(void* data, int count, MPI_Datatype datatype, int root,
              MPI_Comm communicator) {
    int world_rank;
    MPI_Comm_rank(communicator, &world_rank);
    int world_size;
    MPI_Comm_size(communicator, &world_size);

    if (world_rank == root) {
        int i;
        for (i = 0; i < world_size; i++) {
            if (i != world_rank) {
                MPI_Send(data, count, datatype, i, 0, communicator);
            }
        }
    } else {
        MPI_Recv(data, count, datatype, root, 0, communicator, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: compare_bcast num_elements num_trials\n");
        exit(1);
    }
    int num_elements = atoi(argv[1]);
    int num_trials = atoi(argv[2]);
    MPI_Init(NULL, NULL);
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    double total_SRbcast_time = 0.0;
    double total_mpi_bcast_time = 0.0;
    int i;
    int* data = (int*)malloc(sizeof(int) * num_elements);
    assert(data != NULL);

    for (i = 0; i < num_trials; i++) {
        // Time my_bcast
        // Synchronize before starting timing
        MPI_Barrier(MPI_COMM_WORLD);
        total_SRbcast_time -= MPI_Wtime();
        SR_bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
        // Synchronize again before obtaining final time
        MPI_Barrier(MPI_COMM_WORLD);
        total_SRbcast_time += MPI_Wtime();

        // Time MPI_Bcast
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time -= MPI_Wtime();
        MPI_Bcast(data, num_elements, MPI_INT, 0, MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);
        total_mpi_bcast_time += MPI_Wtime();
    }

    // Print off timing information
    if (rank == 0) {
        printf("Data size = %d bytes, Trials = %d times\n", num_elements * (int)sizeof(int),
               num_trials);
        printf("Avger SR_bcast time = %lf seconds\n", total_SRbcast_time / num_trials);
        printf("Avg MPI_Bcast time = %lf seconds\n", total_mpi_bcast_time / num_trials);
    }

    MPI_Finalize();
}

/* -bash-4.1$ mpicc -o Demobcast Demo_bcast2.c
 *  -bash-4.1$ mpirun -n 2 ./Demobcast 1000000 10
Data size = 4000000 bytes, Trials = 10 times
Avger SR_bcast time = 0.001045 seconds
Avg MPI_Bcast time = 0.000719 seconds
*/
