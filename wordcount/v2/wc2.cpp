#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>
#include<stdbool.h>

#define MAX_LINES 100
#define MAX_LINE_LENGTH 256
const char* end_comm = "END";// flag pra indicar o fim da troca de mensagens, pois o MPI nao tem uma forma automatica de "dizer q acabou"

int world_rank;
int world_size;
int linecount = 0;
int root = 0; // salvei o rank do rootpois ele quem estara no MPI_Send enviado

FILE* openFile(const char* filename, const char* mode)
{
    FILE* fp = fopen(filename, mode);
    if (fp == NULL) 
    { 
        fprintf(stderr, "FILE* openFile(...): Nao foi possivel abrir o arquivo.\n");
        MPI_Abort(MPI_COMM_WORLD, 1); // adicionei o MPI_Abort pra uma saida correta pro MPI
        exit(-1); // VERIFICAR: nao sei se faz sentido manter o exit
    }
    return fp;
}

int wordCounter(char* line)
{
    int wc = 0;
    bool isInWord = false;
    for(int i = 0; i < strlen(line); i++)
    {
        if ((line[i] == ' ' && i != 0) || line[i] == '\0' || line[i] == '\t')
        {
            if (isInWord)
            {
                wc++;
                isInWord = false;
            }
        } else {
            isInWord = true;
        }
    }

    if (isInWord) wc++;
    //printf("wordCounter(): A linha %s possui %d palavra(s).\n", line, wc);
    return wc;
}

void readFile(FILE *fp)
{
    char line[MAX_LINE_LENGTH];
    int dest_rank = 1;
    int wordCounterPerLine = 0;
    while(fgets(line, sizeof(line), fp))
    {

        linecount++;
        // wordCounterPerLine = wordCounter(line); // esta fazendo errado. Estava (manualmente) calculando e mandando pros processos
        // eu devo enviar a linha e nao o resultado da contagem
        MPI_Send(line, strlen(line) + 1, MPI_CHAR, dest_rank, 0, MPI_COMM_WORLD);
        dest_rank++;
        if (dest_rank >= world_size)
            dest_rank = 1;
    }

    for(int i = 1; i < world_size; i++)
        MPI_Send("END", 4, MPI_CHAR, i, 0, MPI_COMM_WORLD);
}

/*
Essa funcao recebe as linhas, contas as palavras e devolve o resultado pro root
É basicamente a rotina dos subprocessos.
*/
void slaveRoutine(void)
{
    char line[MAX_LINE_LENGTH];
    int localWc = 0;
    MPI_Status status; // O ultimo parametro do recv serve pra capturar info sobre a msg recebida
    // as infos sao: de que processo veio (MPI_SOURCE), qual tag (MPI_TAG), qtd elementos vieram, etc.
    while(true)
    {
        // Aparentemente preciso limpar o buffer pq ele nao esta sendo sobrescrito corretamente
        /*
        memset preenche todos os bytes do array 'line' com zero (0x00), logo entende-se que
        ele apaga todos os dados antigos que possam ter sobrado dos dados anteriores
        */
        memset(line, 0, sizeof(line));
        MPI_Recv(line, MAX_LINE_LENGTH, MPI_CHAR, root, 0, MPI_COMM_WORLD, &status);
    if (strcmp(line, "END") == 0)
        {
            printf("FLAG END. Encerrando a troca de mensagens.\n");
            break;
        }
        printf("slaveRoutine(): RANK %d recebeu: %s\n", world_rank, line);
        localWc = wordCounter(line);
        MPI_Send(&localWc, 1, MPI_INT, root, 0, MPI_COMM_WORLD);
    }
}

/*
Aqui eu coleto o resultado dos workers, acumulo e retorno o total
Essa função remete aos processos de volta ao ROOT.
*/
int globalWordCount(void)
{
    int globalWc = 0;
    int localWc = 0; // local word count
    MPI_Status status;

    for(int i = 0; i < linecount; i++) {
        MPI_Recv(&localWc, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        printf("globalWordCount(): recebido de RANK %d -> %d palavras\n", status.MPI_SOURCE, localWc);
        globalWc += localWc;
    }

    return globalWc;
}

int main(void)
{
    int result;
    FILE* fp;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if (world_rank == root)
    {
        printf("----------------------------------\n");
        printf("(ROOT) RANK %d\n", world_rank);
        fp = openFile("file2.txt", "r");
        readFile(fp);
        result = globalWordCount();
        printf("main(): Total global de palavras: %d\n", result);
        printf("----------------------------------\n");
    } else {
        printf("----------------------------------\n");
        printf("(SLAVE) RANK %d\n", world_rank);
        slaveRoutine();
        printf("----------------------------------\n");
    }

    // o processo que abriu o arquivo (no caso o root(0)) eh o que deve FECHAR o arquivo
    // se os outros processos tentarem fechar o ponteiro aberto pelo rank 0 vai dar erro (signal 6)
    if (fp != NULL && world_rank == root)
        fclose(fp);

    MPI_Finalize();
    return 0;
}