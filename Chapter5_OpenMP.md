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

## Scope of variable

main 함수에서 선언된 변수들은 parallel 디렉티브에 의해 시작된 모든 스레드들이 액세스할 수 있다. parallel 블록 전에 선언된 변수들의 기본적인 범위는 shared이다.
정리하면, parallel 디렉티브 전에 선언된 변수들은 팀 안에 있는 스레드들 사이에서 공유 범위를 갖는 변수이며, 블록에서 선언된 변수(함수의 지역 변수와 같은)들은 private 범위를 갖는 변수들이다.

다른 디렉티브를 사용하여 변수의 기본 범위를 변경하는 방법은 클라우즈를 통해 가능하다.

## Reduction Clause

위 사다리꼴 법칙을 크리티컬 섹션을 포함하지 않는 함수로 대체하면

```c++ {.line-numbers}
global_result = 0.0;
# pragma omp parallel num_threads(thread_count)
{
  # pragma omp critical
  global_result += Local_trap(double a, double b, int n);
  /* Local_trap 함수 내부에는 critical section이 없고, 부분 면적만 리턴한다. */
}
```

하지만 위 코드는 문제가 존재한다. 크리티컬 섹션을 함수 밖에 정해놓아서 Local_trap 함수를 가동하는데 직렬화된 흐름으로 진행하게 된다. 따라서 런타임 체크해 보면 하나의 스레드를 실행하는 것보다 여러 개의 스레드가 실행하는 것이 더 느릴 수도 있다.

parallel 블록 안에 개인 변수를 선언하고 함수 호출 이후로 크리티컬 섹션을 이동하여 이러한 문제를 피할 수도 있다.

```c++ {.line-numbers}
global_result = 0.0;
# pragma omp parallel num_threads(thread_count)
{
  double my_result = 0.0; /* private variable */
  my_result += Local_trap(double a, double b, int n);
  # pragma omp critical
  global_result += my_result;
}
```

Local_trap의 호출은 크리티컬 섹션 밖에 있고 스레드들은 Local_trap을 순차적으로 호출한다. my_result는 parallel 블록에 선언되었기 때문에 개인 변수이고, 크리티컬 섹션 전에 각 스레드는 계산 결과를 my_result 변수에 저장한다.

> **순차 실행(In-order Execution) vs. 비순차 실행(Out-of-order Execution)**
>
> CPU의 실행 속도를 빠르게 하기 위해 여러 기술들이 등장했고, 비순차 실행은 그 중 하나입니다.
>
>옛날에는 CPU가 착해서 새치기를 봐주지 않았습니다. CPU는 명령어가 들어오는 순서대로 그 명령어를 처리했는데, 이를 순차 실행(In-order) 이라고 합니다.
>
>그런데 시간이 지나면서, 사람들이 일부 명령어는 새치기를 해도 괜찮다는 걸 알게 되었습니다.
>
>1. A = 10
>2. B = 30
>3. C = A * B + 10
>4. D = A * C + 10
>5. E = A + 20
>
>이런 식으로 명령어가 들어온다고 하면, 어떤 명령어가 어떤 명령어 보다 먼저 수행되어야 할까요? 다음과 같은 순서는 반드시 지켜져야 합니다.
>
>1 -> 3 / 1 -> 4  / 1 -> 5 / 2 -> 3 / 3 -> 4
>
>바꿔 말하면 위의 순서가 지켜지기만 하면, 명령어가 어떤 순서가 되건 상관 없다는 이야기입니다. 예를 들어서
>
>1. A = 10
>2. B = 30
>3. C = A * B + 10
>4. E = A + 20
>5. D = A * C + 10
>
>이렇게 실행해도 결과는 같습니다.
>
> reference: [비순차 실행(Out-of-order)은 뭘까요? - CPU의 처리 속도를 빠르게 하기 위한 수단들 (2), Nocher의 천변만화](http://egloos.zum.com/nocher/v/27255)

OpenMP는 Local_trap의 순차적 실행을 피하기 위해서 대안을 제공한다. 그것은 global_result를 reduction 변수로 설정하는 것이다. reduction operator는 이진 연산(덧셈 혹은 곱셈과 같은)이며 reduction은 하나의 결과를 얻기 위해 연속된 피연산자를 동일한 감소 연산자를 적용하여 반복적으로 실행하는 것을 말한다. 연산의 모든 중간 결과값은 같은 변수에 저장된다. 그것이 reduction variable이다. 예를 들어, A가 n개의 정수형 배열이라면,

```c++ {.line-numbers}
int sum = 0;
for (i = 0; i < n; i++)
  sum += A[i];
```

위의 코드는 감소 연산자가 "덧셈"인 감소이다.

OpenMP에서는 감소의 결과가 감소 변수로 설정하도록 하는 것이 가능하다. 이렇게 하기 위해서는 reduction clause가 parallel directive에 추가된다.

```c++ {.line-numbers}
global_result = 0.0;
# pragma omp parallel num_threads(thread_count) \
  reduction(+: global_result)
global_result += Local_trap(double a, double b, int n);
```

위 코드는 global_result가 감소 변수이며 (+)는 감소 연산자가 덧셈이라는 것을 나타낸다.

reduction clause의 문법은 다음과 같다.

```
reduction(<operator>:<variable list>)
```

operator는 `+, *, -, &, |, ^, &&, ||` 중의 하나를 말하며, 뺄셈은 associative or commutive law가 성립되지 않기 때문에 약간의 문제가 있다. 예를 들어,

```c++
result = 0;
for (i = 1; i <= 4; i++)
  result -= i;
```

result에 -10 값이 저장되어야 한다. 그러나 이 연산을 두 개의 반복문으로 나누면 스레드 0에서 연산은 1 - 2이고, 스레드 1에서 연산은 3 - 4이다. 이후 스레드 0의 계산 값 -3과 스레드 1의 계산 값 -7을 뺄셈 연산하면 -3 - (-7) = 4 가 된다.

감소 변수가 float이나 double인 경우도 생각해 보자. 그 결과는 사용되는 스레드의 숫자에 따라 약간 달라진다. 이것은 부동소수점 연산이 결합 규칙이 성립하지 않기 때문이다.

## Parallel for directive

```c++ {.line-numbers}
/* File:    omp_trap3.cpp
 * Purpose: Estimate definite integral (or area under curve) using the
 *          trapezoidal rule.  This version uses a parallel for directive
 *
 * Input:   a, b, n
 * Output:  estimate of integral from a to b of f(x)
 *          using n trapezoids.
 *
 * Compile: g++ -g -Wall -fopenmp -o omp_trap3 omp_trap3.cpp
 * Usage:   ./omp_trap3 <number of threads>
 *
 * Notes:   
 *   1.  The function f(x) is hardwired.
 *   2.  In this version, it's not necessary for n to be
 *       evenly divisible by thread_count.
 *
 * IPP:  Section 5.5 (pp. 224 and ff.)
 */

#include <iostream>
#include <cstdlib>
#include <math.h>
#include <omp.h>

void Usage(char* prog_name);
double f(double x);    /* Function we're integrating */
double Trap(double a, double b, int n, int thread_count);

int main(int argc, char* argv[]) {
   double  global_result = 0.0;  /* Store result in global_result */
   double  a, b;                 /* Left and right endpoints      */
   int     n;                    /* Total number of trapezoids    */
   int     thread_count;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   printf("Enter a, b, and n\n");
   scanf("%lf %lf %d", &a, &b, &n);

   global_result = Trap(a, b, n, thread_count);

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
 * Return val:
 *    approx:  estimate of integral from a to b of f(x)
 */
double Trap(double a, double b, int n, int thread_count) {
   double  h, approx;
   int  i;

   h = (b-a)/n;
   approx = (f(a) + f(b))/2.0;
#  pragma omp parallel for num_threads(thread_count) \
      reduction(+: approx)
   for (i = 1; i <= n-1; i++)
     approx += f(a + i*h);
   approx = h*approx;

   return approx;
}  /* Trap */
```

### Things that you need to be aware of

-   OpenMP는 for loop만을 병렬화 한다. while 이나 do-while loop는 병렬화하지는 않는다.
-   반복문의 횟수가 정해져 있는 for loop만을 병렬화 한다.

```
for (;;;) { ... }

for (i = 0; i < n; i++) { if (...) break; ... }

for (i = 0; i < n; i++) { if (...) return; ... }
```

이와 같은 "무한 루프", "break", "return"같은 비정형화된 형태(non canonical form)은 병렬화 할 수 없다.

-   변수 index는 정수형이거나 포인터형이어야 한다. (e.g. float형은 안 된다.)
-   start, end, 그리고 incr 문장은 호환가능한 타임이어야 한다. (e.g. index가 포인터라면 incr는 정수형이 되어야 한다.)
-   start, end, incr는 루프를 실행하는 동안 변경되서는 안 된다.
-   루프가 실행되는 동안, 변수 index는 for문의 "증감 표현식"에 의해서만 수정될 수 있다.
```
for (index = star; index < end; index++)

사용가능한 조건문

index < end; index <= end; index >= end; index > end;

사용가능한 증감 표현식

index++; index--; ++index; --index; index += incr; index -= incr; index = index + incr; index = incr + index; index = index - incr;

for 문 내부에서 index를 바꾸면 안 된다.
```

위에서 설명한 규칙 중 하나라도 만족하지 못하면 컴파일러는 해당코드를 reject한다.

### Data Dependences (loop-carried dependences)

반복문의 한 번 반복한 결과가 이전 혹은 그 이전의 반복문의 결과에 따라 달라진다면 심각한 문제가 발생한다. 피보나치 수열을 계산한다고 한면,

```c++ {.line-numbers}
fibo[0] = fibo[1] = 1;
#pragma omp parallel for num_threads(thread_count)
for (i = 2; i < n; i++)
  fibo[i] = fibo[i - 1] + fibo[i - 2];
```

위 코드를 컴파일하면 별다른 오류 없이 실행 파일이 실행된다. 그러나 하나 이상의 스레드를 사용하여 실행해 보면 결과가 생각과는 다르다는 것을 알 수 있다.

```
예상 결과: 1 1 2 3 5 13 21 34 55
실행 결과: 1 1 2 3 5  8  0  0  0
```

이는 러타임 시스템은 fibo[2], fibo[3], fibo[4], fibo[5]에 대한 계산을 스레드 0에 할당했고, fibo[6], fibo[7], fibo[8], fibo[9]를 스레드 1에 할당했다. 프로그램이 실행될 때 스레드 0의 연산이 끝나면 문제가 없다. 그러나 스레드 1 연산을 시작하기 이전에 스레드 0이 끝났는지 확신할 수가 없다.

### Calculation of PI

> [Leibniz formula for pi](https://en.wikipedia.org/wiki/Leibniz_formula_for_%CF%80)

$$
\pi = 4[1 - \frac{1}{3} + \frac{1}{5} - \frac{1}{7} + ... ] = 4\sum^{\infty}_{k=0}\frac{(-1)^k}{2k+1}
$$

```c++
double factor = 1.0;
double sum = 0.0;
#pragma omp parallel for num_threads(thread_count) \
  reduction(+:sum)
for (k = 0; k < n; k++) {
  sum += factor / (2k + 1);
  factor = -factor;
}
pi_approx = 4.0 * sum;
```

위 코드에서 문제점은 factor가 각 단계에서 의존성이 존재하기 때문에 값이 올바르게 대입될지 확신할 수가 없다는 것이다.

이 경우에는 if 문(혹은 삼항연산자)을 사용하여 의존성을 제거할 수도 있다.

```c++
if (k %2 == 0)
  factor = 1.0;
else
  factor = -1.0;
```

```c++
factor = (k % 2 == 0) ? 1.0 : -1.0;
```


하지만 이 경우에도 factor 변수는 shared 이기 때문에 루프에 의한 의존성이 제거됐다고 하더라도 자신만의 값은 존재하지 않게 된다. 따라서 factor는 자신의 값을 갖도록 해야 한다. 이 문제는 parallel for directive에 private clause를 추가하면 해소된다.

```c++ {.line-numbers}
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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

void Usage(char* prog_name);

int main(int argc, char* argv[]) {
   long long n, i;
   int thread_count;
   double factor;
   double sum = 0.0;

   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   n = strtoll(argv[2], NULL, 10);
   if (thread_count < 1 || n < 1) Usage(argv[0]);

#  pragma omp parallel for num_threads(thread_count) \
      reduction(+: sum) private(factor)
   for (i = 0; i < n; i++) {
      factor = (i % 2 == 0) ? 1.0 : -1.0;
      sum += factor/(2*i+1);
#     ifdef DEBUG
      printf("Thread %d > i = %lld, my_sum = %f\n", my_rank, i, my_sum);
#     endif
   }

   sum = 4.0*sum;
   printf("With n = %lld terms and %d threads,\n", n, thread_count);
   printf("   Our estimate of pi = %.14f\n", sum);
   printf("                   pi = %.14f\n", 4.0*atan(1.0));
   return 0;
}  /* main */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> <n>\n", prog_name);  /* Change */
   fprintf(stderr, "   thread_count is the number of threads >= 1\n");  /* Change */
   fprintf(stderr, "   n is the number of terms and should be >= 1\n");
   exit(0);
}  /* Usage */
```

그리고, private scope를 갖는 변수의 값은 블록이 시작할 때 그 값을 알 수 없고, 블록이 끝나고 나서도 그 값을 알 수 없다.

예를 들어,

```c++
int x = 5;
#pragma omp parallel num_threads(thread_count) \
  private(x)
{
  int my_rank = omp_get_thread_num();
  printf("x\n", x);
  x = 2*my_rank + 2;
}
printf("x\n", x);
```

이때 블록 시작하고 첫 번째 printf에서의 x 값을 알 수 없고, 블록이 끝나고 printf에서의 x 값을 알 수 없다.

### Scope of variable (default)

OpenMP를 사용하여 각 변수의 범위를 결정하기 전에 개발자로서 블록에 있느 각 변수의 범위를 설정하는 연습을 해 볼 필요가 있다. 여기에 default clause가 사용된다.

```c++
#pragma omp for shared(변수명)
/* 변수명에 지정된 변수를 병렬 영역내의 thread가 공유하며, shared memory에 선언.
기본 설정이기도 하다. */
#pragma omp for private(변수명)
/* thread내의 로컬 메모리에 지정된 변수 선언되며 이 영역은 독립적이므로
Serial 영역의 값이 전달되지 않는다.
private변수는 개별 thread에서 초기화하여 사용해야 하며 초기화 하지 않을 경우
0 또는 쓰레기 값으로 설정된다.
private 지시어로 선언된 변수는 thread 종료와 함께 소멸된다.
만약 Serial 영역에 있는 변수와 이름이 같은 private 변수가 있다 하더라도
서로 완전히 다른 변수처럼 동작한다.*/
#pragma omp for parallel default(none or shared or private ...)
/* 병렬 영역에 있는 글로벌 변수들의 기본 속성을 정한다.
변수의 유효 데이터 범위 속성 설정
자동으로 공유속성이 되는 걸 방지한다. */
```
