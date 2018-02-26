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


### MPI_Send

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

### MPI_Recv

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
