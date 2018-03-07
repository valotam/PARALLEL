#include "mpi.h"
#include <iostream>

int main(int argc, char *argv[])		
{
#define MSGLEN 2048
  int ITAG_A = 100,ITAG_B = 200; 
  int irank, i, idest, isrc, istag, iretag;
  float rmsg1[MSGLEN];
  float rmsg2[MSGLEN];
  MPI::Status irstatus, isstatus;
  MPI::Request request;

  MPI::Init(argc, argv);
  irank = MPI::COMM_WORLD.Get_rank();

  for (i = 1; i <= MSGLEN; i++)
    {
      rmsg1[i] = 100;
      rmsg2[i] = -100;
    }
  if ( irank == 0 )
    { 
      idest  = 1;
      isrc   = 1;
      istag  = ITAG_A;
      iretag = ITAG_B;
    }
  else if ( irank == 1 )
    {
      idest  = 0;
      isrc   = 0;
      istag  = ITAG_B;
      iretag = ITAG_A;
    }

  cout << "Task "  << irank << " has sent the message" << endl;
  request = MPI::COMM_WORLD.Isend(rmsg1, MSGLEN, MPI::FLOAT, idest, istag); 
  MPI::COMM_WORLD.Recv(rmsg2, MSGLEN, MPI::FLOAT, isrc, iretag, irstatus);
  MPI_Wait(request, isstatus);
  cout << "Task " << irank << " has received the message" << endl;
  MPI::Finalize();
}
