#include <iostream>
#include <omp.h>

//#define WORK_IN_PARALLEL

int main() {
    clock_t start, end;

#ifdef WORK_IN_PARALLEL
#pragma omp parallel
    {
        std::cout << "Hello world" << std::endl;
    }
#else
    {
        std::cout << "Hello world" << std::endl;
    }
#endif // WORK_IN_PARALLEL


    return 0;
}

