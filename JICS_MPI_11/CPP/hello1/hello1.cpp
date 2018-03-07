#include "mpi.h"
#include <iostream>

int main(int argc, char** argv)	
{
  int rank;

  // Initialize MPI
  MPI::Init(argc, argv);

  // Find out my rank in the global communicator MPI_COMM_WORLD
  rank = MPI::COMM_WORLD.Get_rank( );

  // Insert code to do conditional work if my rank is 0 
  if(rank == 0){
    cout << "Hello, World!!! (from masternode)" << endl;

  } else{

  // Insert code to print the output message "Hello, World!"
  cout << "Hello, World! (from worker node)" << endl;

  }
  // Exit and finalize MPI
  MPI::Finalize();		
}
