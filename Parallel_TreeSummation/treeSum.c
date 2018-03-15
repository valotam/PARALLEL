#include <stdio.h>
#include "mpi.h"


typedef struct node{
    int num;
    int assigned_rank;

    node* left_node;
    node* right_node;
}Node

int main(int argc, const char* argv[]) {
    int sum, node_num;

    MPI_Init(&argc, &argv);

    MPI_Finalize();
    return 0;
}
