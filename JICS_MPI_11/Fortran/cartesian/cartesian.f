      program cartesian
      include 'mpif.h'

      integer SIZE, UP, DOWN, LEFT, RIGHT
      parameter(SIZE=16)
      parameter(UP=1)
      parameter(DOWN=2)
      parameter(LEFT=3)
      parameter(RIGHT=4)
      integer numtasks, rank, source, dest, outbuf, i, tag, ierr,
     &        inbuf(4), nbrs(4), dims(2), coords(2),
     &        stats(MPI_STATUS_SIZE, 8), reqs(8), cartcomm,
     &        periods(2), reorder
      data inbuf /MPI_PROC_NULL,MPI_PROC_NULL,MPI_PROC_NULL,
     &     MPI_PROC_NULL/,  dims /4,4/, tag /1/, 
     &     periods /0,0/, reorder /0/ 

C      *----------------*
C     * Initialize MPI *
C    *----------------*

      call MPI_INIT(ierr)

C      *-------------------------------------------------------*
C     * Get the size of the MPI_COMM_WORLD communicator group *
C    *-------------------------------------------------------*

      call MPI_COMM_SIZE(MPI_COMM_WORLD, numtasks, ierr)
  
      if (numtasks .eq. SIZE) then

C      *---------------------------------------------------------------------*
C     * Make a new communicator to which 2-D Cartesian topology is attached *
C    *---------------------------------------------------------------------*

        call MPI_CART_CREATE(MPI_COMM_WORLD, 2, dims, periods, reorder,
     &                        cartcomm, ierr)

C      *------------------------------------------*
C     * Get my rank in the cartcomm communicator *
C    *------------------------------------------*

        call MPI_COMM_RANK(cartcomm, rank, ierr)

C      *--------------------------------------------------------------------*
C     * Determine process coords in cartesian topology given rank in group *
C    *--------------------------------------------------------------------*

        call MPI_CART_COORDS(cartcomm, rank, 2, coords, ierr)

C      *--------------------------------------------------------------------*
C     * Obtain the shifted source and destination ranks in both directions *
C    *--------------------------------------------------------------------*

        call MPI_CART_SHIFT(cartcomm, 0, 1, nbrs(UP),nbrs(DOWN),ierr)
        call MPI_CART_SHIFT(cartcomm, 1, 1, nbrs(LEFT),nbrs(RIGHT),ierr)

        outbuf = rank
        do i=1,4
          dest = nbrs(i)
          source = nbrs(i)
C      *----------------------------------------------*
C     * send messages to the four adjacent processes *
C    *----------------------------------------------*
          call MPI_ISEND(outbuf, 1, MPI_INTEGER, dest, tag,
     &                    MPI_COMM_WORLD, reqs(i), ierr)
C      *---------------------------------------------------*
C     * receive messages from the four adjacent processes *
C    *---------------------------------------------------*
          call MPI_IRECV(inbuf(i), 1, MPI_INTEGER, source, tag,
     &                    MPI_COMM_WORLD, reqs(i+4), ierr)
        enddo

C      *------------------------------------------------*
C     * Wait for all 8 communication tasks to complete *
C    *------------------------------------------------*

        call MPI_WAITALL(8, reqs, stats, ierr)

        write(*,"('rank = ',i2,' coords = ',2(i2),
     &  ' neighbors(u,d,l,r) = ',4(i3))")rank,coords,nbrs

        write(*,"('rank = ',i2,
     &  '                   inbuf(u,d,l,r) = ',4(i3))")rank,inbuf

      else
        print *, 'Must specify',SIZE,' processors.  Terminating.' 
      endif

C      *--------------*
C     * Finalize MPI *
C    *--------------*

      call MPI_FINALIZE(ierr)
      end
