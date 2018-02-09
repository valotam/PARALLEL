#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>
#include <math.h>
#include <time.h>

int main() {
    clock_t start, end;

    std::vector<int> vec ={3, 2, 1, 4, 5, 6, 10, 8, 9, 4};

    // sequential as ever
    start = clock();
    std::sort(vec.begin(), vec.end());
    end = clock();
    std::cout << "Sequential as ever time: " << end - start << "ms" << std::endl;

    // sequential
    start = clock();
    std::sort(std::execution::seq, vec.begin(), vec.end());
    end = clock();
    std::cout << "Sequential time: " << end - start << "ms" << std::endl;

    // parallel
    start = clock();
    std::sort(std::execution::par, vec.begin(), vec.end());
    end = clock();
    std::cout << "Parallel time: " << end - start << "ms" << std::endl;

    // parallel and vectorized
    start = clock();
    std::sort(std::execution::par_unseq, vec.begin(), vec.end());
    end = clock();
    std::cout << "Parallel and vectorized time: " << end - start << "ms" << std::endl;

    return 0;

}
