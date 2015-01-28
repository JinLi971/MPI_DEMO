/* File: Demo_pio.c
 * This file illustrates how to use MPI_File_read_all and MPI_File_write_ordered
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>

/* Test reading and writing zero bytes (set status correctly) */

int main( int argc, char *argv[] )
{
    int errs = 0;
    int size, rank, i, *buf, count;
    MPI_File fh;
    MPI_Comm comm;
    MPI_Status status;

    MPI_Init( &argc, &argv );

    comm = MPI_COMM_WORLD;
    errs = MPI_File_open( comm, "/home/jing.liu/work/lect/parpro15/mpi_demo/build/test.ord", MPI_MODE_WRONLY | MPI_MODE_CREATE , MPI_INFO_NULL, &fh );
    if(errs != 0 )
        printf("file failed at %d",rank);
    MPI_Comm_size( comm, &size );
    MPI_Comm_rank( comm, &rank );
    buf = (int *)malloc( size * sizeof(int) );
    buf[0] = rank + 1024;

    /* Write to file */
    MPI_File_write_ordered( fh, buf, 1, MPI_INT, &status );
    MPI_Get_count( &status, MPI_INT, &count );
    if (count != 1) {
        errs++;
        fprintf( stderr, "Wrong count (%d) on write-ordered\n", count );
        fflush(stderr);
    }
    MPI_File_close( &fh );
    errs = MPI_File_open( comm, "/home/jing.liu/work/lect/parpro15/mpi_demo/build/test.ord", MPI_MODE_RDONLY  , MPI_INFO_NULL, &fh );
    if(errs != 0 )
        printf("file failed at %d",rank);
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

