#include <iostream>
#include <chrono>
#include <ctime>
#include <math.h>
#include <omp.h>

#define WORK_IN_PARALLEL

int main() {
    const size_t SIZE = 1000000000;
    float *arr = new float[SIZE];

    std::clock_t c_start = std::clock();
    auto t_start = std::chrono::high_resolution_clock::now();

#ifdef WORK_IN_PARALLEL
#pragma omp parallel
    {
#pragma omp for
        for (size_t ii = 0; ii < SIZE; ii++) {
            arr[ii] = sqrt(ii);
        }
    }
#else
    for (size_t ii = 0; ii < SIZE; ii++) {
        arr[ii] = sqrt(ii);
    }
#endif // WORK_IN_PARALLEL

    std::clock_t c_end = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();

    std::cout << std::fixed << "CPU time used: "
              << 1000.0 * (c_end-c_start) / CLOCKS_PER_SEC << " ms\n"
              << "Wall clock time passed: "
              << std::chrono::duration<double, std::milli>(t_end-t_start).count()
              << " ms\n";
    return 0;


    /*
    // parallel
    CPU time used: 18745.4 ms
    Wall clock time passes: 4824.4 ms

    execution time: 4.926 s

    // serial
    CPU time used: 4428.8 ms
    Wall clock time passes: 4428.8 ms

    execution time: 4.522 s

    // parallel for
    CPU time used: 4714.5 ms
    Wall clock time passes: 1338.2 ms

    execution time: 1.438 s
    */
}
