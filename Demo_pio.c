/* File: Demo_pio.c
 * This file illustrates how to use MPI_File_read_all and MPI_File_write_ordered
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015, last update: Jan 2016
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <unistd.h>

int main( int argc, char *argv[] )
{
    int errs = 0;
    int size, rank, i, *buf, count;
    MPI_File fh;
    MPI_Comm comm;
    MPI_Status status;

    MPI_Init( &argc, &argv );

    comm = MPI_COMM_WORLD;

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd() error");
        MPI_Finalize();
        return 0;
    }
    char cwd_in[1024];
    sprintf (cwd_in, "%s/test.ord", cwd);
    const char * path_in =  (const char * )  &cwd_in;

    /*write file*/
    //the file path cannot be a relative path
    errs = MPI_File_open( comm, path_in, MPI_MODE_WRONLY | MPI_MODE_CREATE , MPI_INFO_NULL, &fh );
    if(errs != 0 )
        printf("Write file %s failed at %d",path_in,rank);
    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    buf = (int *)malloc( size * sizeof(int) );
    buf[0] = rank ;

    /* Write to file */
    MPI_File_write_ordered( fh, buf, 1, MPI_INT, &status );
    MPI_Get_count( &status, MPI_INT, &count );
    if (count != 1) {
        errs++;
        fprintf( stderr, "Wrong count (%d) on write-ordered\n", count );
        fflush(stderr);
    }
    MPI_File_close( &fh );


    /*read file*/
    errs = MPI_File_open( comm, path_in, MPI_MODE_RDONLY  , MPI_INFO_NULL, &fh );
    if(errs != 0 )
        printf("Read file %s failed at %d",path_in,rank);

    /* Set the individual pointer to 0, since we want to use a read_all */
    MPI_File_seek_shared( fh, 0, MPI_SEEK_SET );
    for (i=0; i<size; i++)
        buf[i] = -1;
    MPI_File_read_ordered( fh, buf, 1, MPI_INT, &status );
    printf(  "read %d: buf = %d\n", rank, buf[0] );
    free( buf );
    MPI_File_close( &fh );


    MPI_Finalize();
    return 0;
}

/*
-bash-4.1$ mpicc -o Demopio ../Demo_pio.c
-bash-4.1$ mpirun -n 4 Demopio
read 0: buf = 0
read 3: buf = 3
read 2: buf = 2
read 1: buf = 1
-bash-4.1$ hexdump test.ord
0000000 0000 0000 0001 0000 0002 0000 0003 0000 0000010
*/
