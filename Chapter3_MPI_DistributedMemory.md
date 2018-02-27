# MPI를 이용한 분산 메모리 프로그래밍

메시지 패싱 프로그램에서 코어-메모리에서 실행되는 프로그램을 보통 process 라고 하고, 두 개의 프로세스는 send 함수라고 하는 프로세스와 receive 함수라고 하는 프로세스를 사용하여 통신한다. 또한, 두 개 이상의 프로세스를 포함하는 "글로벌" 통신, 함수들을 collective 통신이라고 한다. MPI 함수들에 대하여 배우는 과정에서 분산 메모리 시스템의 데이터 파티셔닝과 I/O 와 같은 메시지 패싱 프로그램 이슈를 포함한 기본적인 이슈에 대해서 배울 것이다. 병렬 프로그램 성능 관련 이슈에 대해서도 알아볼 것이다.

## Hello, world!

```cpp {.line-numbers}
#include <stdio.h>
#include <string.h>
#include <mpi.h>

const int MAX_STRING = 100;

int main(void) {
  char  greeting[MAX_STRING];
  int   comm_sz;    /* the number of processes */
  int   my_rank;    /* process' ranking number */

  MPI_Init(NULL, NULL);
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
```

각 프로세스가 간단하게 메시지를 출력하는 대신 하나의 프로세스가 출력을 맡고 다른 프로세스는 메시지를 전송하도록 설계했다. 병렬 프로그램에서 음수가 아닌 정수형 rank 를 사용하여 프로세스를 식별하는 것이 일반적이다. p 개의 프로세스들이 있다면 그 프로세스들은 0, 1, 2, ... , p-1 의 등급을 갖고 있다. 병렬로 "hello, world"를 출력하도록 만들기 위해서 프로세스 0이 메시지를 출력하도록 하고 다른 프로세스는 메시지를 전송하도록 설계했다.

### Build and Run

컴파일 명령어: mpicc (wrapper script for c compiler) ($ mpicc -g -Wall -o mpi_hello mpi_hello.c)

실행 명령어: mpiexec ($ mpiexec -n \<number of processes> ./mpi_hello)


#### MPI_Send

```cpp {.line-numbers}
int MPI_Send(
  void*         msg_buf_p     /* in */,
  int           msg_size      /* in */,
  MPI_Datatype  msg_type      /* in */,
  int           dest          /* in */,
  int           tag           /* in */,
  MPI_Comm      communicator  /* in */);
```

첫 번째 세 개의 인수인 `msg_buf_p`, `msg_size`와 `msg_type` 은 메시지의 콘텐츠를 결정한다. 나머지 인수인 `dest`, `tag`, 그리고 `communicator`는 메시지의 목적지를 결정한다.

-   `msg_bug_p` 는 메시지의 내용, 즉 콘텐츠를 갖고 있는 메모리 블록의 포인터이다. 이것은 메시지를 포함하고 있는 문자열이다.
-   `msg_size`와 `msg_type` 은 전송할 데이터의 양을 결정한다. 프로그램에서 `msg_size` 인수는 메시지에 포함되어 있는 문자의 수와 NULL 값 (`\0`) 하나를 더한 숫자로, `strlen(greeting) + 1` 이다. `msg_type` 인수는 MPI_CHAR 이다.

| Data type for MPI  | Data type for C  |
|---|---|
| MPI_CHAR  | signed char  |
| MPI_SHORT   | signed short int  |
| MPI_INT   | signed int  |
| MPI_LONG   | singed long int  |
| MPI_LONG_LONG   | singed long long int   |
| MPI_UNSIGNED_CHAR   | unsigned char   |
| MPI_UNSIGNED_SHORT   | unsigned short int   |
| MPI_UNSIGNED   | unsigned int  |
| MPI_UNSIGNED_LONG   | unsigned long int   |
| MPI_FLOAT  | float   |
| MPI_DOUBLE   | double   |
| MPI_LONG_DOUBLE   | long double   |
| MPI_BYTE   |  |
| MPI_PACKED   | |

-   `dest` 는 메시지를 수신하는 프로세스의 랭크를 설정한다.
-   `tag`는 음수가 아닌 `int` 형이다. 메시지를 식별하는데 사용된다. 예를 들어, 프로세스 1이 프로세스 0에 실수형을 전송한다고 가정해 보자. 실수형 몇몇은 출력되지만 다른 몇몇은 연산을 위해 사용된다. 그렇다면 MPI_Send 의 처음 네 개의 인수들은 어떤 실수형이 출력되고 어떤 실수형이 연산에 사용될지에 대한 정보를 알려 줄 수가 없다. 따라서 프로세스 1은 태그를 0으로 되어 있는 메시지는 출력을 하고, 태그가 1로 되어 있는 메시지는 연산에 사용하도록 설정할 수 있다.
-   `communicator` 는 커뮤니케이터로, 통신을 포함하고 있는 모든 MPI 함수들은 커뮤니케이터 인수를 갖고 있다. 커뮤니케이터가 서로 간에 메시지를 전송하는 프로세스들의 컬렉션이라는 사실을 기억하자. 반대로 하나의 커뮤니케이터를 사용하는 프로세스에 의해 전송되는 메시지는 다른 커뮤니케이터를 사용하는 프로세스에 의해 수신될 수 없다. 이를 통해 "우연히 수신됐다"라는 상황이 발생하지 않도록 한다.

#### MPI_Recv

```cpp {.line-numbers}
int MPI_Recv(
  void*         msg_buf_p     /* out */,
  int           buf_size      /* in */,
  MPI_Datatype  buf_type      /* in */,
  int           source        /* in */,
  int           tag           /* in */,
  MPI_Comm      communicator  /* in */,
  MPI_Status*   status_p      /* out */);
```


## MPI를 사용한 사다리꼴 규칙 (Trapezoidal rule)

![](http://www.cse.iitd.ernet.in/~mittal/gate/trapezoidal_general.gif)

> pseudo-code for serial computing

```cpp {.line-numbers}
/* Input: a, b, n */
h = (b-a) / n;
approx = (f(a) + f(b)) / 2.0;
for (i = 1; i <= n - 1; i++){
  x_i = a + i * h;
  approx += f(x_i);
}
approx = h * approx;
```

이를 네 개의 기본 과정을 사용하여 병렬 프로그램으로 설계한다면 아래와 같다.

1.  문제의 솔루션을 태스크로 분할한다.
2.  태스크 간의 통신 채널을 식별한다.
3.  태스크를 혼합 태스크로 묶는다.
4.  혼합 태스크를 코어에 매핑한다.

```mermaid
graph TD
    id1((Calculation trap 0 to 3)) --> id4((Summation all of traps))
    id2((Calculation trap 4 to 7)) --> id4
    id5((...)) --> id4
    id3((Calculation trap n-3 to n)) --> id4
```

> pseudo-code for parallel computing

```cpp {.line-numbers}
Get a, b, n;
h = (b-a) / n;
local_n = n / comm_sz;
local_a = a + my_rank * local_n * h;
local_b = local_a + local_n * h;
local_integral = Trap(local_a, local_b, local_n, h);
if (my_rank != 0) {
  Send local_integral to process 0;
}
else /* my_rank == 0 */ {
  total_integral = local_integral;
  for (proc = 1; proc < comm_sz; proc++){
    Receive local_integral from proc;
    total_integral += local_integral;
  }
}
if (my_rank == 0)
  print result;
```

> c code

```c {.line-numbers}
int main(void) {
  int my_rank, comm_sz, n = 1024, local_n;
  double a= 0.0, b = 3.0, h, local_a, local_b;
  double local_int, total_int;
  int source;

  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

  h = (b - a) / n;        /* all the processes have same h value */
  local_n = n / comm_sz; /* the number of trapezoids */

  local_a = a + my_rank * local_n * h;
  local_b = local_a + local_n * h;
  local_integral = Trap(local_a, local_b, local_n, h);

  if (my_rank != 0) {
    MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
  } else {
    total_int = local_int;
    for (source = 1; source < comm_sz; source++){
      MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      total_int += local_int;
    }
  }

  if (my_rank == 0) {
    printf("With n = %d trapezoids, our estimate\n", n);
    printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
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
```

## 컬렉티브 통신

사다리꼴 프로그램에서 성능을 향상시킬 수 있는 부분이 존재한다.
가장 명확한 부분은 각 프로세스가 연산한 후에 통합하는 "글로벌 합계" 부분이다. 예를 들어 8명의 근로자를 고용했다고 가정하고, 이 근로자들이 집을 짓는다고 가정하자. 근로자들 중에서 7명에게 그들이 해야 할 일을 지시한 후에 그에 대한 보수를 지급하고 근로자들을 귀가시킨다. 이것은 글로벌 합계에서 우리가 하는 것과 매우 비슷하다. 랭크가 0보다 큰 프로세스들은 "프로세스 0에게 해야 할 일을 말한다" 그리고 종료한다. 랭크가 0인 프로세스는 "이 숫자를 합계에 더하라"라고 말한다. 프로세스 0은 모든 작업 결과를 글로벌 합계에 더하고 다른 프로세스는 이제 거의 아무것도 하지 않는다. 그러나 8명의 근로자들에게 더 적합한 작업 분배를 고려해 낼 수 있다.

### 트리 구조 통신

```mermaid
graph TD
  5 -.- n8
  n8 -.- n10
  n10 -.- n20

  n2["-1"] -.- n9
  6 -.- n7
  n0["-7"] -.- n6

  n7 -.- n11

  2 --> n8[7]
  n3["-3"] --> n9["-4"]
  n1[5] --> n7[11]
  n4[2]--> n6["2"]

  n9 --> n10[3]
  n6 --> n11[6]

  n11 --> n20[9]
```

이러한 솔루션은 이상적으로 보이지 않는다. 그 이유는 프로세스의 절반(1,3,5,7)은 기존의 방식 그대로 작업하기 때문이다.(다른 프로세스들이 일하는 동안 쉬고 있다.)

### MPI_Reduce

"글로벌 합계 함수"는 통신이 필요하다. 그러나 MPI_Send와 MPI_Recv의 쌍과는 달리 글로벌 합계 함수는 두 개 이상의 프로세스를 포함한다. 사실 사다리꼴 규칙 프로그램에서 MPI_COMM_WORLD의 모든 프로세스를 포함한다. MPI의 용법에서는 커뮤니케이터에 있는 모든 프로세스를 포함하는 통신 함수를 **collective communication** 이라고 한다. MPI_Send와 MPI_Recv를 **point-to-point communication** 이라고 한다.

사실 글로벌 합계는 컬렉티브 통신의 한 경우이다. 예를 들어 합계가 아닌, 최댓값, 최솟값 등의 기능을 할 경우도 생기기 때문에, 이러한 컬렉티브 통신의 일반화 함수를 구현했다.

```c {.line-numbers}
int MPI_Reduce(
    void*         input_data_p    /* in  */,
    void*         output_data_p   /* out */,
    int           count           /* in  */,
    MPI_Datatype  datatype        /* in  */,
    MPI_Op        operator        /* in  */,
    int           dest_process    /* in  */,
    MPI_Comm      comm            /* in  */);
  )
```

> MPI에서 사용하는 미리 정의된 리덕션 오퍼레이터

| Operation value  | Definition  |
|---|---|
| MPI_MAX  | maximum  |
| MPI_MIN   | minimum  |
| MPI_SUM   | summation   |
| MPI_PROD   | product   |
| MPI_LAND   | logical and   |
| MPI_BAND   | bitwise and   |
| MPI_LOR   | logical or   |
| MPI_BOR   | bitwise or   |
| MPI_LXOR   | logical exclusive or   |
| MPI_BXOR   | bitwise exclusive or   |
| MPI_MAXLOC   | maximum and location of maximum   |
| MPI_MINLOC   | minimum and location of minimum   |

예를 들어, 아래의 코드를 리덕션으로 작성한다면

```c {.line-numbers}
if (my_rank != 0) {
  MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
} else {
  total_int = local_int;
  for (source = 1; source < comm_sz; source++){
    MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    total_int += local_int;
  }
}
```

다음과 같다.

```c {.line-numbers}
MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
```

그리고, 다음의 코드는 하나의 프로세당 N 차원 벡터의 컬렉션을 덧셈하는 데 사용된다.

```c {.line-numbers}
double local_x[N], sum[N];
...
MPI_Reduce(local_x, sum, N, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
```

### 컬렉티브 통신 대 일대일 통신

컬렉티브 통신이 여러 가지 면에서 일대일 통신과 다르다.

1.   커뮤니케이터 안에 있는 모든 프로세스는 같은 컬렉티브 함수를 호출해야 한다. 예를 들어, MPI_Reduce에 대한 호출은 다른 프로세스에서 MPI_Recv에 대한 호출과 매칭 시키려는 시도는 에러를 발생시킬 수 있다. 프로그램이 hang 되거나 crash 된다.
2.   각 프로세스에서 MPI 컬렉티브 통신으로 전달된 인수들은 "호환"되어야 한다. 예를 들어, dest_process로서 0을 전달하고 다른 프로세스에는 1을 전달하면 MPI_Reduce의 호출에 대한 결과는 에러를 발생시킨다. 이런 경우도 프로그램이 행되거나 크래시된다.
3.   output_data_p 인수는 dest_process에서만 사용되어야 한다. 그러나 모든 프로세스는 여전히 output_data_p에 대응되는실제 인수를 넘길 필요가 있다. NULL을 사용해도 된다.
4.   일대일 통신은 tag와 communicator를 기반으로 매칭된다. 컬렉티브 통신은 태그를 사용하지 않으며 커뮤니케이터와 호출된 순서를 기반으로 매칭된다. 예를 들어, 아래 표에서 각 프로세스가 MPI_SUM 오퍼레이터를 갖고 MPI_Reduce를 호출하고 목적 프로세스는 0이라고 가정하자. 얼핏 보면 MPI_Reduce에 대한 두 번의 호출이 발생한 것처럼 보인다. b의 값은 3이고 d의 값은 6이다. 그러나 메모리 위치의 이름은 매칭이나 MPI_Reduce의 호출과는 상관이 없다. 호출 순서가 매칭을 결정한다. 따라서 b에 저장된 값은 1 + 2 + 1 = 4이며 d에 저장된 값은 2 + 1 + 2 = 5이다.

time  | process 0  | call  | process 1  | call  | process 2  | call
--|---|---|---|---|---|--
0  | a = 1, c = 2;  |   | a = 1, c = 2;  |   | a = 1, c = 2;  |  
1  | MPI_Reduce(&a, &b, ...)  | 1  | MPI_Reduce(&c, &d, ...)  | 1  | MPI_Reduce(&a, &b, ...)  | 1
2  | MPI_Reduce(&c, &d, ...)  | 2  | MPI_Reduce(&a, &b, ...)  | 2  | MPI_Reduce(&c, &d, ...)  | 2

주의: 입력과 출력을 같은 버퍼를 사용하여 MPI_Reduce를 호출하려고 할 수있다. 예를 들어, 각 프로세스에서 x의 글로벌 합계를 구해서 프로세스 0의 x에 결과를 저장하려고 한다면 다음과 같이 호출할 수 있다.

```c {.line-numbers}
MPI_Reduce(&x, &x, 1, MPI_DOUBLE, MPI_SUM, 0, comm);
```

그러나 이 호출은 결과를 예측할 수 없다. 출력 인수의 alias를 포함하고 있기 때문에 맞지 않는 코드이다. 두 인수가 같은 메모리 블록을 참조한다면 앨리어스되고, MPI는 그들 중 하나가 입력 혹은 입력/출력 인수라면 인수의 앨리어스를 금지하고 있다.

### MPI_Allreduce

하나의 프로세스가 글로벌 합계의 결과를 필요한 상황 말고도, 모든 프로세스에서 글로벌 합계가 필요한 상황이 있을 수도 있다. 이러한 경우에 원래의 글로벌 합계에서 직면했던 것과 같은 문제를 만나게 된다. 예를 들어, 글로벌 합계를 구하기 위해 트리를 사용한 경우, 글로벌 합계를 분산하기 위해 브랜치를 "역으로" 구성해야 한다.

![reverse](http://cs.umw.edu/~finlayson/class/fall16/cpsc425/notes/images/all-reduce1.png)

반대로 한 방향으로 통신하는 대신 부분적인 결과를 바꾸는 방법이 있는데, 이러한 통신 패턴은 보통 butterfly라고 한다.

![butterfly](http://cs.umw.edu/~finlayson/class/fall16/cpsc425/notes/images/all-reduce2.png)

다행히 어떤 구조를 사용할지 최적의 성능을 위해 어떻게 코딩할지를 결정할 필요는 없다. MPI_Allreduce를 제공하기 때문이다.

```c {.line-numbers}
int MPI_Allreduce(
    void*         input_data_p    /* in  */,
    void*         output_data_p   /* out */,
    int           count           /* in  */,
    MPI_Datatype  datatype        /* in  */,
    MPI_Op        operator        /* in  */,
    MPI_Comm      comm            /* in  */);
  )
```

### Broadcast

하나의 프로세스에 속해 있는 데이터의 컬렉티브 통신은 커뮤니케이터에 있는 모든 프로세스에게 전송되고 이런 방식을 broadcast라고 한다.

```c {.line-numbers}
int MPI_Bcast(
    void*         data_p        /* in/out  */,
    int           count         /* in  */,
    MPI_Datatype  datatype      /* in  */,
    int           source_proc   /* in  */,
    MPI_Comm      comm          /* in  */);
  )
```

랭크 source_proc을 갖고 있는 프로세스는 커뮤티케이터 comm에 있는 모든 프로세스에게 data_p에 의해 참조되는 메모리의 내용을 전송한다.
