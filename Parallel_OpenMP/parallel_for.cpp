/* File:     omp_pi.cpp
 * Purpose:  Estimate pi using OpenMP and the formula
 *
 *              pi = 4*[1 - 1/3 + 1/5 - 1/7 + 1/9 - . . . ]
 *
 * Compile:  g++ -g -Wall -fopenmp -o omp_pi omp_pi.cpp -lm
 * Run:      omp_pi <thread_count> <n>
 *           thread_count is the number of threads
 *           n is the number of terms of the series to use
 *
 * Input:    none
 * Output:   The estimate of pi and the value of pi computed by the
 *           arctan function in the math library
 *
 * Notes:
 *    1.  The radius of convergence is only 1.  So the series converges
 *        *very* slowly.
 *
 * IPP:   Section 5.5.4 (pp. 229 and ff.)
 */

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <omp.h>

void Usage(char* prog_name);

int main(int argc, char* argv[])
{
    long long n, i;
    int thread_count;
    double factor;
    double sum = 0.0;

    if (argc != 3)
        Usage(argv[0]);
    std::istringstream ss(argv[1]);
    if (!(ss >> thread_count))
        std::cerr << "Invalid number " << argv[1] << std::endl;
    std::istringstream ss2(argv[2]);
    if (!(ss2 >> n))
        std::cerr << "Invalid number " << argv[2] << std::endl;
    if (thread_count < 1 || n < 1)
        Usage(argv[0]);
    std::cout.precision(15);

    #  pragma omp parallel for num_threads(thread_count) \
    reduction(+: sum) private(factor)
    for (i = 0; i < n; i++)
    {
        factor = (i % 2 == 0) ? 1.0 : -1.0;
        sum += factor/(2*i+1);
#     ifdef DEBUG
        std::cout << "Thread "<< my_rank << " > i = " << i << ", my_sum = " << my_sum << std::endl;
#     endif
    }
    sum = 4.0*sum;
    std::cout << "With n = "<< n << " terms and " << thread_count << " threads," << std::endl;
    std::cout << "   Our estimate of pi = " << sum << std::endl;
    std::cout << "                   pi = " << 4.0*atan(1.0) << std::endl;
    return 0;
}  /* main */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name)
{
    std::cerr << "usage: " << prog_name << " <thread_count> <n>" << std::endl; /* Chage */
    std::cerr << "   thread_count is the number of threads >= 1" << std::endl; /* Chage */
    std::cerr << "   n is the number of terms and should be >= 1" << std::endl;
    exit(0);
}  /* Usage */
