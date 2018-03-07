      program group
      include 'mpif.h'

      integer NPROCS
      parameter(NPROCS=12)
      integer rank, new_rank, sendbuf, recvbuf, numtasks
      integer ranks1(6), ranks2(6), ierr
      integer orig_group, new_group, new_comm
      data ranks1 /0, 1, 2, 3, 4, 5/, ranks2 /6, 7, 8, 9, 10, 11/

C     *----------------*
C    * Initialize MPI *
C   *----------------*

      call MPI_INIT(ierr)

C     *------------------------------------------------------*
C    * Get my rank in the MPI_COMM_WORLD communicator group *
C   *------------------------------------------------------*

      call MPI_COMM_RANK(MPI_COMM_WORLD, rank, ierr)

C     *-------------------------------------------------------*
C    * Get the size of the MPI_COMM_WORLD communicator group *
C   *-------------------------------------------------------*

      call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)

      if (numtasks .ne. NPROCS) then
        print *, 'Must specify MPROCS= ',NPROCS,' Terminating.'
        call MPI_FINALIZE(ierr)
        stop
      endif

      sendbuf = rank

C     *-----------------------------------*
C    * Extract the original group handle *
C   *-----------------------------------*

      call MPI_COMM_GROUP(MPI_COMM_WORLD, orig_group, ierr)

C     *-------------------------------------------------------*
C    * Divide tasks into two distinct groups based upon rank *
C   *-------------------------------------------------------*

      if  (rank .lt. NPROCS/2) then
         call MPI_GROUP_INCL(orig_group, NPROCS/2, ranks1, 
     &                  new_group, ierr)
      else 
      call MPI_GROUP_INCL(orig_group, NPROCS/2, ranks2, 
     &                  new_group, ierr)
      endif

C     *------------------------------------------------------------------------*
C    * Create new new communicator and then perform collective communications *
C   *------------------------------------------------------------------------*

      call MPI_COMM_CREATE(MPI_COMM_WORLD, new_group, 
     &                  new_comm, ierr)
      call MPI_ALLREDUCE(sendbuf, recvbuf, 1, MPI_INTEGER,
     &                  MPI_SUM, new_comm, ierr)

C     *-----------------------------------------------------*
C    * Get my rank in the new new_group communicator group *
C   *-----------------------------------------------------*

      call MPI_GROUP_RANK(new_group, new_rank, ierr)

      print *, 'rank= ',rank,' newrank= ',new_rank,' recvbuf= ',
     &     recvbuf

C     *--------------*
C    * Finalize MPI *
C   *--------------*

      call MPI_FINALIZE(ierr)

      end
