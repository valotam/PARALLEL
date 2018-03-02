#include <stdio.h>
#include <time.h>
#include <math.h>

double Trap(
        double  left_endpt    /* in */,
        double  right_endpt   /* in */,
        int     trap_count    /* in */,
        double  base_len      /* in */);

double f(double endpt);

int main(void) {
    int n = 2048;
    double a= 0.0, b = 3.0, h;
    double total_int;


    int result;
    struct timespec tp;
    clockid_t clk_id;

    clk_id = CLOCK_MONOTONIC;

    result = clock_gettime(clk_id, &tp);
    unsigned long int t_start = tp.tv_nsec;
    h = (b - a) / n;        /* all the processes have same h value */

    total_int = Trap(a, b, n, h);

    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.15e\n", a, b, total_int);

    result = clock_gettime(clk_id, &tp);
    t_start = tp.tv_nsec - t_start;;
    printf("TIME: %lf\n", (double)t_start/1000000);



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



