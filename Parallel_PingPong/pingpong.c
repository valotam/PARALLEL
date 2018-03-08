#include	<stdio.h>
#include	<mpi.h>

#define PROC_A 0
#define PROC_B 1
#define PING 101
#define PONG 101
#define MAX_SIZE 10001
#define STRIDE 1000

float buffer[MAX_SIZE];

void processor_A (void);
void processor_B (void);

void main ( int argc, char *argv[] )
{
     int ierror, rank, size;

     printf("===Ping pong\n");

   /*----------------*/
  /* Initialize MPI */
 /*----------------*/

     MPI_Init(&argc, &argv);

   /*--------------------------*/
  /* Get my process ID number */
 /*--------------------------*/

     MPI_Comm_rank(MPI_COMM_WORLD, &rank);

     if (rank == PROC_A) processor_A();
     else if (rank == PROC_B) processor_B();

   /*--------------*/
  /* Finalize MPI */
 /*--------------*/

     MPI_Finalize();

}

void processor_A( void )
{

    int i, ierror;
    int sfloat;
    MPI_Status status;

    double time;

    extern float buffer[MAX_SIZE];

    int length;

    sfloat = sizeof(float);

    printf("length   time/message (sec)    transfer rate (byte/sec)\n");

   /*---------------------------------*/
  /* Process A sets the message size */
 /*---------------------------------*/

     for (length = 1; length <=  MAX_SIZE; length += STRIDE){

   /*-----------------------------------------------------*/
  /* Get the start time for the pingpong message passing */
 /*-----------------------------------------------------*/

        time = MPI_Wtime();

   /*--------------------------------------------------------------*/
  /* Process A sends and then receives the message back 100 times */
 /*--------------------------------------------------------------*/

        for (i = 1; i <= 100; i++){

            MPI_Ssend(buffer, length, MPI_FLOAT, PROC_B, PING,
                      MPI_COMM_WORLD);

            MPI_Recv(buffer, length, MPI_FLOAT, PROC_B, PONG,
                     MPI_COMM_WORLD, &status);

        }

   /*------------------------------------------------------*/
  /* Get the finish time for the pingpong message passing */
 /*------------------------------------------------------*/

        time = MPI_Wtime() - time;

        printf("%d\t %6.6f\t\t%20.6f\n", length, time/200., 2 * sfloat * 100 * ((float)length)/time);

    }

}

void processor_B( void )
{
    int i, ierror;
    MPI_Status status;

    extern float buffer[MAX_SIZE];

    int length;

   /*---------------------------------*/
  /* Process B sets the message size */
 /*---------------------------------*/

    for (length = 1; length <= MAX_SIZE; length += STRIDE) {

   /*--------------------------------------------------------------*/
  /* Process B receives and then sends the message back 100 times */
 /*--------------------------------------------------------------*/

            for (i = 1; i <= 100; i++) {

                MPI_Recv(buffer, length, MPI_FLOAT, PROC_A, PING,
                         MPI_COMM_WORLD, &status);

                MPI_Ssend(buffer, length, MPI_FLOAT, PROC_A, PONG,
                         MPI_COMM_WORLD);

            }

    }

}
