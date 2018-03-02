#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

double Trap(
        double  left_endpt    /* in */,
        double  right_endpt   /* in */,
        int     trap_count    /* in */,
        double  base_len      /* in */);

double f(double endpt);

int main(void) {
  int my_rank, comm_sz, n = 2048, local_n;
  double a= 0.0, b = 3.0, h, local_a, local_b;
  double local_int, total_int;
  //int source;

  double ticktock = MPI_Wtime();
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  h = (b - a) / n;        /* all the processes have same h value */
  local_n = n / comm_sz; /* the number of trapezoids */

  local_a = a + my_rank * local_n * h;
  local_b = local_a + local_n * h;
  local_int = Trap(local_a, local_b, local_n, h);

  /* reduction
  if (my_rank != 0) {
    MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
    total_int = local_int;
    for (source = 1; source < comm_sz; source++){
      MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      total_int += local_int;
    }
  } */
  MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

  if (my_rank == 0) {
    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
    ticktock = MPI_Wtime() - ticktock;

    printf("TIME: %f\n", ticktock);
  }

  MPI_Finalize();

  return 0;
} /* main */

double Trap(
      double  left_endpt    /* in */,
      double  right_endpt   /* in */,
      int     trap_count    /* in */,
      double  base_len      /* in */) {
  double estimate, x;
  int i;

    estimate = (f(left_endpt) + f(right_endpt)) / 2.0;
    for (i = 1; i <= trap_count - 1; i++) {
      x = left_endpt + i * base_len;
      estimate += f(x);
    }
    estimate = estimate * base_len;

    return estimate;
} /* Trap */

double f(double endpt) {
    return abs(3*sin(endpt)) + 5;
} /* f */
