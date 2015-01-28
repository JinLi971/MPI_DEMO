/* File: Demo_prime.c
 * This is an example for to compute prime numbers using MPI
 * Author: Jing Liu @ TDB,LMB, Uppsala University
 * Contact: jing.liu@it.uu.se , jing.liu@icm.it.uu.se
 * Date: Jan, 2015
*/
# include <stdlib.h>
# include <stdio.h>
# include <math.h>
# include <time.h>

# include "mpi.h"


/*****************************************************************************
    In order to divide the work up evenly among P processors, processor
    ID starts at 2+ID and skips by P.
    A naive algorithm is used.
    Mathematica can return the number of primes less than or equal to N
    by the command PrimePi[N].
                N  PRIME_NUMBER
                1           0
               10           4
              100          25
            1,000         168
           10,000       1,229
          100,000       9,592
        1,000,000      78,498
       10,000,000     664,579
      100,000,000   5,761,455
    1,000,000,000  50,847,534
******************************************************************************/

int main ( int argc, char *argv[] );
int prime_number ( int n, int id, int p );
void timestamp ( void );

int main ( int argc, char *argv[] )
{
    int id, ierr,n,n_factor,n_hi,n_lo,p,primes,primes_part;
    double wtime=0;

    n_lo = 1;
    n_hi = atoi(argv[1]);
    MPI_Init ( &argc, &argv );
    MPI_Comm_size ( MPI_COMM_WORLD, &p );
    MPI_Comm_rank ( MPI_COMM_WORLD, &id );
    n_factor =p ;

    if ( id == 0 )
    {
        timestamp ( );
        printf ( "\n" );
        printf ( "PRIME_MPI\n" );
        printf ( "  C/MPI version\n" );
        printf ( "\n" );
        printf ( "  An MPI example program to count the number of primes.\n" );
        printf ( "  The number of processes is %d\n", p );
        printf ( "\n" );
        printf ( "         N        #prime          Time\n" );
        printf ( "\n" );
    }
    n = n_lo;
    while ( n <= n_hi )
    {
        if ( id == 0 )
        {
            wtime = MPI_Wtime ( );
        }
        ierr = MPI_Bcast ( &n, 1, MPI_INT, 0, MPI_COMM_WORLD );

        primes_part = prime_number ( n, id, p );

        ierr = MPI_Reduce ( &primes_part, &primes, 1, MPI_INT, MPI_SUM, 0,
                            MPI_COMM_WORLD );
        if ( id == 0 )
        {
            wtime = MPI_Wtime ( ) - wtime;
            printf ( "  %8d  %8d  %14f\n", n, primes, wtime );
        }
        n = n * n_factor;
    }
    if ( id == 0 )
    {
        printf ( "\n");
        printf ( "PRIME_MPI - Master process:\n");
        printf ( "  Normal end of execution.\n");
        printf ( "\n" );
        timestamp ( );
    }
    MPI_Finalize ( );
    return 0;
}

int prime_number ( int n, int id, int p )
{
    int i;
    int j;
    int prime;
    int total;
    total = 0;
    for ( i = 2 + id; i <= n; i = i + p )
    {
        prime = 1;
        for ( j = 2; j < i; j++ )
        {
            if ( ( i % j ) == 0 )
            {
                prime = 0;
                break;
            }
        }
        total = total + prime;
    }
    return total;
}
void timestamp ( void )
{
# define TIME_SIZE 40
    static char time_buffer[TIME_SIZE];
    const struct tm *tm;
    size_t len;
    time_t now;
    now = time ( NULL );
    tm = localtime ( &now );
    len = strftime ( time_buffer, TIME_SIZE, "%d %B %Y %I:%M:%S %p", tm );
    printf ( "%s\n", time_buffer );
    return;
# undef TIME_SIZE
}
