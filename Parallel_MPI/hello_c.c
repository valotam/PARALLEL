/* It just show the number of threads.
 * In the for loop, All of works have not distributed as much as the threads,
 * but the master thread only do it.
 * It means, all of threads except master send messages abroad, and master sends it.
 */

#include <stdio.h>
#include <string.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main(int argc, const char* argv[]) {
  char  greeting[MAX_STRING];
  int   comm_sz;    /* the number of processes */
  int   my_rank;    /* process' ranking number */

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

  if (my_rank != 0) {
    sprintf(greeting, "Greetings from process %d of %d!", my_rank, comm_sz);
    MPI_Send(greeting, strlen(greeting) + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
  } else {
    printf("Greetings from process %d of %d!\n", my_rank, comm_sz);
    for (int q = 1; q < comm_sz; q++) {
      MPI_Recv(greeting, MAX_STRING, MPI_CHAR, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      printf("%s\n", greeting);
    }
  }

  MPI_Finalize();
  return 0;
}

/* OpenMPI example
 * To run OpenMPI code:
 *   1. Set compiler's settings as "GNU GCC COMPILER MPI."
 *   2. Specify searching directory for linkers, /usr/mpi/lib, where it was downloaded.
 */

//#include <stdio.h>
//#include "mpi.h"
//
//int main(int argc, char* argv[])
//{
//    int rank, size, len;
//    char version[MPI_MAX_LIBRARY_VERSION_STRING];
//
//    MPI_Init(&argc, &argv);
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &size);
//    MPI_Get_library_version(version, &len);
//    printf("Hello, world, I am %d of %d, (%s, %d)\n",
//           rank, size, version, len);
//    MPI_Finalize();
//
//    return 0;
//}
