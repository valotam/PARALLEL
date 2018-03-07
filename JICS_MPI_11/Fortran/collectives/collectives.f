      program collectives

      implicit none

      include "mpif.h"

      integer p, ierr, i, Iam, root
      character*1 x(0:3), y(0:3), alphabets(0:15), a
      character*1 yv(0:9)
      integer counts(4), displs(4)
      data counts/1,2,3,4/
      data displs/0,1,3,6/
      data alphabets/'a','b','c','d','e','f','g','h','i','j','k','l',
     &               'm','n','o','p'/
      data root/1/

C       *----------------*
C      * initialize MPI *
C     *----------------*

      call MPI_Init(ierr)

C       *---------------------------*
C      * get the process ID number *
C     *---------------------------*

      call MPI_Comm_rank(MPI_COMM_WORLD, Iam, ierr)

C       *-----------------------------------*
C      * get the size of the process group *
C     *-----------------------------------*

      call MPI_Comm_size(MPI_COMM_WORLD, p, ierr)

      if (Iam .eq. 0) then
        write(*,*)'  Function      Proc      Sendbuf         Recvbuf'
        write(*,*)'  --------      ----      -------         -------'
      endif
      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C       *-----------------------------------*
C      * MPI_Gather()                      *
C     *-----------------------------------*

      a = alphabets(Iam)

      do i=0,p-1
        y(i) = ' '
      enddo

      call MPI_Gather(a,1,MPI_CHARACTER,       ! send buf,count,type
     &                y,1,MPI_CHARACTER,       ! recv buf,count,type
     &                root,                    ! root (data origin)
     &                MPI_COMM_WORLD,ierr)     ! comm,flag
      
      write(*,"('MPI_Gather    :',t20,i2,(3x,a1),t40,4(3x,a1))")Iam,a,y

      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C       *-----------------------------------*
C      * MPI_Gatherv()                     *
C     *-----------------------------------*

      do i=0,3
        x(i) = ' '
      enddo

      do i=0,9
        yv(i) = ' '
      enddo

      do i=0,Iam
        x(i) = alphabets(displs(Iam+1)+i)
      enddo

      call MPI_Gatherv(x,counts(Iam+1),MPI_CHARACTER, ! send buf,count,type
     &                 yv,counts,                     ! recv buf,count array
     &                 displs,MPI_CHARACTER,          ! displacements,type
     &                 root,                          ! root (data origin)
     &                 MPI_COMM_WORLD,ierr)           ! comm,flag

      write(*,"('MPI_Gatherv   :',t20,i2,4(3x,a1),t40,10(3x,a1))")
     &  Iam,x,yv
                                                                                                                                   
      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C       *-----------------------------------*
C      * MPI_Allgather()                   *
C     *-----------------------------------*

      a = alphabets(Iam)

      do i=0,p-1
        y(i) = ' '
      enddo
      call MPI_Allgather(a,1,MPI_CHARACTER,    ! send buf,count,type
     &                   y,1,MPI_CHARACTER,    ! recv buf,count,type
     &                   MPI_COMM_WORLD,ierr)  ! comm,flag
      
      write(*,"('MPI_Allgather :',t20,i2,(3x,a1),t40,4(3x,a1))")Iam,a,y

      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C       *-----------------------------------*
C      * MPI_Allgatherv()                  *
C     *-----------------------------------*

      do i=0,3
        x(i) = ' '
      enddo

      do i=0,9
        yv(i) = ' '
      enddo

      do i=0,Iam
        x(i) = alphabets(displs(Iam+1)+i)
      enddo

      call MPI_Allgatherv(x,counts(Iam+1),MPI_CHARACTER, ! send buf,count,type
     &                    yv,counts,                     ! recv buf,count array
     &                    displs,MPI_CHARACTER,          ! displacements,type
     &                    MPI_COMM_WORLD,ierr)           ! comm,flag

      write(*,"('MPI_Allgatherv:',t20,i2,4(3x,a1),t40,10(3x,a1))")
     &  Iam,x,yv
                                                                                                                                   
      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C       *-----------------------------------*
C      * MPI_Scatter()                     *
C     *-----------------------------------*

      do i=0,p-1
        x(i) = alphabets(i+Iam*p)
        y(i) = ' '
      enddo

      call MPI_Scatter(x,1,MPI_CHARACTER,      ! send buf,count,type
     &                 y,1,MPI_CHARACTER,      ! recv buf,count,type
     &                 root,                   ! data origin
     &                 MPI_COMM_WORLD,ierr)    ! comm,flag

      write(*,"('MPI_Scatter   :',t20,i2,4(3x,a1),t40,4(3x,a1))")Iam,x,y

      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C   *-----------------------------------*
C  * MPI_Alltoall()                    *
C *-----------------------------------*

      do i=0,p-1
        x(i) = alphabets(i+Iam*p)
        y(i) = ' '
      enddo

      call MPI_Alltoall(x,1,MPI_CHARACTER,     ! send buf,count,type
     &                  y,1,MPI_CHARACTER,     ! recv buf,count,type
     &                  MPI_COMM_WORLD,ierr)   ! comm,flag

      write(*,"('MPI_Alltoall  :',t20,i2,4(3x,a1),t40,4(3x,a1))")Iam,x,y

      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C   *-----------------------------------*
C  * MPI_Reduce()                      *
C *-----------------------------------*

      do i=0,p-1
        x(i) = alphabets(i+Iam*p)
        y(i) = ' '
      enddo

      call MPI_Reduce(x,y,                 ! send buf,send recv 
     &                p,MPI_CHARACTER,     ! count,type
     &                MPI_MAX,             ! operation
     &                root,                ! data origin
     &                MPI_COMM_WORLD,ierr) ! comm,flag

      write(*,"('MPI_Reduce MAX:',t20,i2,4(3x,a1),t40,4(3x,a1))")Iam,x,y

      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C   *-----------------------------------*
C  * MPI_Allreduce()                   *
C *-----------------------------------*

      do i=0,p-1
        x(i) = alphabets(i+Iam*p)
        y(i) = ' '
      enddo

      call MPI_Allreduce(x,y,              ! send buf,send recv 
     &                p,MPI_CHARACTER,     ! count,type
     &                MPI_MAX,             ! operation
     &                MPI_COMM_WORLD,ierr) ! comm,flag

      write(*,"('MPI_Allreduce :',t20,i2,4(3x,a1),t40,4(3x,a1))")Iam,x,y

      call MPI_Barrier(MPI_COMM_WORLD,ierr)

C       *-----------------------------------*
C      * MPI_Bcast()                       *
C     *-----------------------------------*

      a = ' '

      do i=0,p-1
        y(i) = ' '
      enddo

      if(Iam .eq. root) then
        a = 'b'
        y(0) = a
      endif

      call MPI_Bcast(a,1,MPI_CHARACTER,        ! buf,count,type
     &               root,MPI_COMM_WORLD,ierr) ! root,comm,flag
      
      write(*,"('MPI_Bcast     :',t20,i2,4(3x,a1),t40,4(3x,a1))")Iam,y,a

C       *--------------*
C      * Finalize MPI *
C     *--------------*

      call MPI_Finalize(ierr)

      end
