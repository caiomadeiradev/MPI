#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>
#include<stdbool.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 256

int world_rank;
int world_size;
float local_sum = 0.0;
float global_sum = 0.0;

FILE* openFile(const char* filename, const char* mode)
{
    FILE* fp = fopen(filename, mode);
    if (fp == NULL) { fprintf(stderr, "unable to open file!\n"); exit(-1); }
    return fp;
}

void readFile(int num_elements_per_proc)
{
    char line[MAX_LINE_LENGTH];
    int linecount = 0;
    int wordcount = 0;
    FILE* fp = openFile("file2.txt", "r");

    while(fgets(line, sizeof(line), fp))
    {
        // count words
        if (linecount % world_size == world_rank)
        {
            bool in_word = false;
            for(int i = 0; i < strlen(line); i++)
            {
                if ((line[i] == ' ' && i != 0) || line[i] == '\0' || line[i] == '\t')
                {
                    if (in_word)
                    {
                        wordcount++;
                        in_word = false;
                    } 
                } 
                else 
                {
                    in_word = true;
                }
            }
    
            if (in_word) 
                wordcount++;
            local_sum += wordcount;
            printf("---------------------------\n");
            printf("Process: %d\n", world_rank);
            printf("Line [%d]: %s\n", linecount, line);
            printf("Word count: %d\n", wordcount);
            printf("Local sum: %.2f\n", local_sum);
            printf("---------------------------\n");
        }
        linecount++;
    }

    fclose(fp);
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int num_elements_per_proc = atoi(argv[1]);

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    readFile(num_elements_per_proc);
    MPI_Reduce(&local_sum, &global_sum, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (world_rank == 0)
    {
        printf("Total de palavras no arquivo (global_sum): %.2f\n", global_sum);        
    }

    MPI_Finalize();
    return 0;
}