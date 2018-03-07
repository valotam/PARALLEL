      program fixed
      implicit none
      include 'mpif.h'
      integer MSGLEN, ITAG_A, ITAG_B
      parameter (MSGLEN = 2048, ITAG_A = 100, ITAG_B = 200)
      real rmsg1(MSGLEN), rmsg2(MSGLEN)
      integer irank, idest, isrc, istag, iretag 
      integer ierr, I, request, irstatus(MPI_STATUS_SIZE)
      integer isstatus(MPI_STATUS_SIZE)

      call MPI_Init (ierr)
      call MPI_Comm_rank( MPI_COMM_WORLD, irank, ierr)
      
      do I = 1, MSGLEN
        rmsg1(I) = 100
        rmsg2(I) = -100
      end do
      
      if ( irank .eq. 0 ) then 
        idest  = 1
        isrc   = 1
        istag  = ITAG_A
        iretag = ITAG_B
      else if ( irank .eq. 1 ) then  
        idest  = 0
        isrc   = 0
        istag  = ITAG_B
        iretag = ITAG_A
      end if
      
      print *, "Task", irank, " has sent the message "
      call MPI_Isend( rmsg1, MSGLEN, MPI_REAL, idest, istag, 
     *  MPI_COMM_WORLD,  request, ierr)
      call MPI_Recv(rmsg2, MSGLEN, MPI_REAL, isrc, iretag, 
     *  MPI_COMM_WORLD, irstatus, ierr)
      call MPI_Wait(request, isstatus, ierr)
      print*, "Task", irank, " has received the message " 
      call MPI_Finalize (ierr)
      end
