#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <mpi.h>

#define SIZE 8

int main(void){
    int comm_sz, my_rank, i, stride, batch_size;
    int per_proc;
    int* arr_num, *sub_arr;
    int sub_sum;
    MPI_Request request;
    MPI_Status status;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        printf("ARRAY: ");
        arr_num = (int *)malloc(sizeof(int) * SIZE);
        for (i = 0; i < SIZE; i++) {
            arr_num[i] = (rand() % 20 - 10);
            printf("%d ", arr_num[i]);
        }
        printf("\n");
    }

    per_proc = SIZE/comm_sz;
    sub_arr = (int *)malloc(sizeof(int) * per_proc);

    MPI_Scatter(arr_num, per_proc, MPI_INT, sub_arr, per_proc, MPI_INT, 0, MPI_COMM_WORLD);

    printf("PROC %d: ", my_rank);
    for(i = 0; i < per_proc; i++) {
        printf("%d ", sub_arr[i]);
        sub_sum += sub_arr[i];
    }
    printf("\n");

    for (i = 0, stride = 1, batch_size = 2; stride < comm_sz; i++, stride<<=1, batch_size<<=1) {
        if ((my_rank % batch_size) == stride) {
            printf("Send %d from proc %d to %d\n", sub_sum, my_rank, my_rank - stride);
            MPI_Ssend(&sub_sum, 1, MPI_INT, my_rank - stride, 0, MPI_COMM_WORLD);
        }
        if ((my_rank % batch_size) == 0) {
            int sum_from_even;
            MPI_Recv(&sum_from_even, 1, MPI_INT, my_rank + stride, 0, MPI_COMM_WORLD, &status);
            sub_sum += sum_from_even;
            printf("Recv %d from proc %d to %d\n", sum_from_even, my_rank + stride, my_rank);
            printf("#%d SUB_SUM: %d by PROC %d\n", i + 1, sub_sum, my_rank);
        }
    }

    if  (my_rank == 0) {
        printf("\n::Total summation is %d::\n", sub_sum);
        free(arr_num);
    }
    free(sub_arr);

    MPI_Finalize();

    return 0;
}
