        PROGRAM hello

        IMPLICIT NONE

        include "mpif.h"

        INTEGER ierror, rank

C       Initialize MPI 
        CALL MPI_INIT(ierror)

C       Find out my rank in the global communicator MPI_COMM_WORLD
        CALL MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierror)

C *-->  Insert code to do conditional work if my rank is 0 
        IF (rank.EQ.0) THEN

          WRITE(*,*)'Hello, World!!! (from masternode)'

        ELSE

C *-->  Insert code to print the output message "Hello, World"
        WRITE(*,*) 'Hello, World! (from worker node)'
        END IF

C       Exit and finalize MPI 
        CALL MPI_FINALIZE(ierror)

        STOP
        END
