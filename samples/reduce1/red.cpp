#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

void printv(int *v, int size)
{
    for(int i = 0; i < size; i++)
    {
        if (i + 1 == size)
        {
            printf("%d\n", v[i]);
        } else { printf("%d,", v[i]); }
    }
}

int main(int argc, char** argv)
{
    int world_size;
    int world_rank;
    float local_sum = 0;
    float global_sum;

    int v[5] = { 1, 2, 3, 4, 5 };
    printv(v, 5);

    int num_elements_per_proc = atoi(argv[1]);
    printf("num_elements_per_proc: %d\n", num_elements_per_proc);

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    for(int i = 0; i < num_elements_per_proc; i++) 
    {
        local_sum += v[i];
    }

    printf("Local sum for process %d - %f\n", world_rank, local_sum);
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
        printf("Global Sum: %f\n", global_sum);

    MPI_Finalize();
    return 0;
}