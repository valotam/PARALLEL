# OpenMP

OpenMP는 공유 메모리 병렬 프로그래밍 API다.

> Pthreads와 차이

Pthreads는 low level이어서 개발자가 thread의 실행 형태를 세밀하게 제어할 수 있도록 한다.
반면, OpenMP는 high level로 간단하게 명시적으로 개발자들이 기존의 시리얼 프로그램을 점차 병렬화 할 수 있도록 설계됐다.

> 배울 것

-   OpenMP를 활용하여 간단하게 for loop 병렬화 시키기
-   task-parallelism 배우기
-   명시적인 스레드 동기화 배우기
-   공유 메모리 프로그래밍에서 캐시 메모리 효과와 시리얼코드-특별히 시리얼 라이브러리-가 공유 메모리 프로그램에서 사용될 때 발생할 수 있는 문제점들에 대한 이해

## 101

OpenMPsms "directive 기반" 공유 메모리 API다. 이는 C와 C++에서 pragma와 같은 특별한 preprocessor 명령어를 의미한다. 기본적인 C에서는 제공되지 않지만, 시스템에 추가하여 특별한 형태의 일을 할 수 있도록 해 주는 기능이다. pragma를 지원하지 않는 컴파일러는 이 기능을 무시한다. pragma를 지원하지 않는 플랫폼에서 pragma를 사용할 수도 있다. 원칙적으로는 주의 깊게 OpenMP 프로그래밍을 하면, 컴파일러가 OpenMP를 지원하느 것과 상관없이 C 컴파일러에 의해 컴파일되고 실행된다.

### Hello OpenMP!

```c++ {.line-numbers}
#include <iostream>
#include <sstream>
#include <omp.h>

void Hello(void); /* thread function */

int main(int argc, char* argv[]) {
    /* Insert the number of threads which are usable in command line */
    std::istringstream ss(argv[1]);
    int thread_count;
    if (!(ss >> thread_count))
        std::cerr << "Invalid number " << argv[1] << std::endl;

    #pragma omp parallel num_threads(thread_count)
    Hello();

    return 0;
} /* main */

void Hello(void) {
    int my_rank = omp_get_thread_num();
    int thread_count = omp_get_num_threads();

    std::cout << "Hello from thread " << my_rank << " of " << thread_count << std::endl;
}
```

> directive

`pragma omp parallel num_threads(thread_count)` OpenMP directive의 의미는 프로그램이 몇 개의 스레드로 시작하라고 명시하는 부분이다. fork 될 스레드는 Hello 함수를 실행하게 되고, 스레드가 Hello 함수 호출을 종료하고 리턴하게 되면 해당 스레드는 종료해야 한다. 여기서 Pthreads와 차이는 Pthreads를 배울 때는 많은 양의 포크를 해야 했고 여러 스레드를 join 해야 했다. 각 스레드에 필요한 특별한 구조체를 위한 메모리 영역도 할당해야 했고, for loop에서 각 스레드를 시작하고 다른 for loop에서 스레드를 종료해야 했다. 하지만 OpenMP에서는 parallel directive를 이용하여, 코드의 structured block이 다중 스레드에 의해 시행된다는 것을 명시한다고 짐작할 수 있다.

> clause

OpenMP에서 clause는 directive를 제어하는 텍스트를 의미한다. 즉, `#pragma omp parallel num_threads(thread_count)`의 `num_threads`를 말한다. `num_threads`clause는 parallel directive에 추가할 수 있다. 위와 같이 실행할 블록을 몇 개의 스레드로 실행할지 설정할 수 있도록 해 준다.

> the number of threads

프로그램이 시작할 때 실행하게 될 스레드의 숫자는 제한이 될 수 있다는 점을 알아 두자. OpenMP 표준에서는 실제로 thread_count 만큼의 스레드를 시작한다는 것을 보장하지는 않는다. 그러나 대부분의 시스템은 수백 혹은 수천 개의 스레드를 시작할 수 있으며, 너무 많은 스레드를 실행하지 않는다면 대부분의 원하는 만큼의 스레드를 시작할 수 있다.

> team with master and slaves

프로그램에 parallel 디렉티브가 있으면 어떤 일이 일어날까? parallel 디렉티브에 앞서 프로그램은 하나의 스레드를 사용하며, 프로그램이 시작할 때 프로세스가 시작한다. 프로그램에 parallel 디렉티브를 만나면 원래의 스레드는 계속 실행하고, thread_count - 1개의 추가 스레드가 시작된다. OpenMP에서는 스레드의 집합이 parallel 블록을 실행하며 원래의 스레드와 새로운 스레드를 한 team이라고 한다. 원래의 스레드를 master라고 하며, 추가적인 스레드를 slave라고 한다. 팀에 있는 각 스레드는 디렉티브 이후에 나오는 블록을 실행하며, 우리의 예제에서는 각 스레드는 Hello 함수를 실행한다.

> implicit barrier

코드 블록의 실행이 완료되면 스레드들은 Hello 함수로부터 리턴한다. 여기에 implicit barrier가 있다. 이 말은 실행을 완료한 코드 블록의 스레드는 한 팀에 속해 있는 나머지 스레드들이 블록을 완료할 때까지 대기하게 된다. 모든 스레드가 블록을 완료하면, 슬레이브 스레드는 종료하게 되고 마스테 스레드는 계속 다음 코드 블록을 실행하게 된다.

> stack and program counter each owned

각 스레드는 자신만의 스택을 갖고 있기 때문에 Hello 함수를 실행하느 스레드는 함수에서 사용하는 자신만의 private variables나 local variables를 생성하게 된다. 예제에서 함수가 호출됐을 때 각 스레드는 OpenMP 함수인 `omp_get_thread_num`과 `omp_get_num_threads` 함수 호출에 의해 rank나 id 그리고 스레드 숫자를 얻게 된다.

> unpredictable results

standard output stream (e.g. cout)에 액세스할 때는 스케줄링되지 않기 때문에 스레드들이 결과물을 출력하는 순서는 알 수 없다.

## Error checking

실제 작업할 때는 예상되는 오류에 대해 해당 오류를 체크하는 코드를 반드시 넣는 것이 필요하다. 이 예제에서는 명령어 라인 매개변수의 존재 유무에 대해 체크해야 한다. 만약 한 개라면 strtol 호출 후에 그 값을 positive라고 체크한다. parallel 디렉티브에 의해 생성되는 스레드의 숫자가 thread_count와 같은지도 체크해야 한다.

발생할 수 있는 두 번째 문제는 컴파일러이다. 컴파일러가 OpenMP를 지원하지 않는다면, 컴파일러는 parallel 디렉티브를 무시할 것이다. 그러나 omp.h를 include 안하고 omp_get_thread_num과 omp_get_num_threads를 호출하면 오류가 발생하게 된다. 이 문제를 해결하기 위해서 전처리 매크로 `_OPENMP`가 정의되었는지를 체크해야 한다. 정의되어 있다면, omp.h를 include하고 OpenMP 함수를 호출할 수 있다.

```c++ {.line-numbers}
/* File:     omp_hello.cpp
 *
 * Purpose:  A parallel hello, world program that uses OpenMP
 *
 * Compile:  g++ -g -Wall -fopenmp -o omp_hello omp_hello.cpp
 * Run:      ./omp_hello <number of threads>
 *
 * Input:    none
 * Output:   A message from each thread
 *
 * Note:     This version does some basic error checking:  it checks
 *           the command line argument, and it checks the number of
 *           threads started by the parallel directive.  It also
 *           checks for availability of OpenMP by testing for the
 *           _OPENMP macro
 *
 * IPP:      Section 5.1.3 (pp. 215 and ff.)
 */
#include <iostream>
#include <cstdlib>
#ifdef _OPENMP
#  include <omp.h>   
#endif _OPENMP

void Usage(char* prog_name);
void Hello(int thread_count);  /* Thread function */

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int thread_count;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   if (thread_count <= 0) Usage(argv[0]);

#  pragma omp parallel num_threads(thread_count)
   Hello(thread_count);

   return 0;
}  /* main */

/*--------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message indicating how program should be started
 *            and terminate.
 */
void Usage(char *prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   fprintf(stderr, "   thread_count should be positive\n");
   exit(0);
}  /* Usage */

/*--------------------------------------------------------------------
 * Function:    Hello
 * Purpose:     Thread function that prints message
 */
void Hello(int thread_count) {
#  ifdef _OPENMP
      int my_rank = omp_get_thread_num();
      int actual_thread_count = omp_get_num_threads();
#  else
      int my_rank = 0;
      int actual_thread_count = 1;
#  endif

   if (my_rank == 0 && thread_count != actual_thread_count)
      fprintf(stderr, "Number of threads started != %d\n", thread_count);
   printf("Hello from thread %d of %d\n", my_rank, actual_thread_count);

}  /* Hello */
```

## Trapezoidal rule

```c++
/* serial codes */
h = (b - a) / n;
approx = (f(a) + f(b)) / 2.0;
for (i = 1; i <= n - 1; i++) {
  x_i = a + i * h
  approx += f(x_i);
}
approx = h*approx;
```

### OpenMP 1st version for calculating trapezoidal rule

위 시리얼 코드를 병렬화 할 경우 approx를 global_result로 계산한다면

```
global_result += my_result;
```

critical section이므로 race condition이 발생한다.
이를 해소하기 위해서 Pthread에서는 mutex와 semaphore를 사용했다.
OpenMP에서는 critical directive를 사용한다.

```
#pragma omp critical
global_result += my_result;
```

이 디렉티브는 시스템으로 하여금 스레드들이 이 디렉티브 다음에 오는 구조화된 블록을 mutually exclusive로 실행하도록 컴파일러에게 지시하는 역할을 한다. 오직 하나의 스레드만이 동시에 디렉티브 다음에 나오는 구조화된 블록을 실행한다.

```c++ {.line-numbers}
/* File:    omp_trap1.cpp
 * Purpose: Estimate definite integral (or area under curve) using trapezoidal
 *          rule.
 *
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: g++ -g -Wall -fopenmp -o omp_trap1 omp_trap1.cpp
 * Usage:   ./omp_trap1 <number of threads>
 *
 * Notes:   
 *   1.  The function f(x) is hardwired.
 *   2.  In this version, each thread explicitly computes the integral
 *       over its assigned subinterval, a critical directive is used
 *       for the global sum.
 *   3.  This version assumes that n is evenly divisible by the
 *       number of threads
 *
 * IPP:  Section 5.2.1 (pp. 216 and ff.)
 */

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <omp.h>

void Usage(char* prog_name);
double f(double x);    /* Function we're integrating */
void Trap(double a, double b, int n, double* global_result_p);

int main(int argc, char* argv[]) {
   double  global_result = 0.0;  /* Store result in global_result */
   double  a, b;                 /* Left and right endpoints      */
   int     n;                    /* Total number of trapezoids    */
   int     thread_count;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   printf("Enter a, b, and n\n");
   scanf("%lf %lf %d", &a, &b, &n);
   if (n % thread_count != 0) Usage(argv[0]);
#  pragma omp parallel num_threads(thread_count)
   Trap(a, b, n, &global_result);

   printf("With n = %d trapezoids, our estimate\n", n);
   printf("of the integral from %f to %f = %.14e\n",
      a, b, global_result);
   return 0;
}  /* main */

/*--------------------------------------------------------------------
 * Function:    Usage
 * Purpose:     Print command line for function and terminate
 * In arg:      prog_name
 */
void Usage(char* prog_name) {

   fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
   fprintf(stderr, "   number of trapezoids must be evenly divisible by\n");
   fprintf(stderr, "   number of threads\n");
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input arg:   x
 * Return val:  f(x)
 */
double f(double x) {
   double return_val;

   return_val = x*x;
   return return_val;
}  /* f */

/*------------------------------------------------------------------
 * Function:    Trap
 * Purpose:     Use trapezoidal rule to estimate definite integral
 * Input args:  
 *    a: left endpoint
 *    b: right endpoint
 *    n: number of trapezoids
 * Output arg:
 *    integral:  estimate of integral from a to b of f(x)
 */
void Trap(double a, double b, int n, double* global_result_p) {
   double  h, x, my_result;
   double  local_a, local_b;
   int  i, local_n;
   int my_rank = omp_get_thread_num();
   int thread_count = omp_get_num_threads();

   h = (b-a)/n;
   local_n = n/thread_count;  
   local_a = a + my_rank*local_n*h;
   local_b = local_a + local_n*h;
   my_result = (f(local_a) + f(local_b))/2.0;
   for (i = 1; i <= local_n-1; i++) {
     x = local_a + i*h;
     my_result += f(x);
   }
   my_result = my_result*h;

#  pragma omp critical
   *global_result_p += my_result;
}  /* Trap */
```
