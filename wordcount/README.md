### Word Count MPI

#### v1 - com MPI_Reduce
Essa primeira versão que fiz está meio errada com a proposta do exercício.

Todos os processos MPI (rank N's) abrem o mesmo arquivo inteiro. __Todos eles executam readFile().__ Com isso, todos abrem o arquivo file2.txt e leem linha por linha, mas só usam algumas linhas pois o cálculo ```ĺinecount % world_size == world_rank``` decide qual processo (rank) vai ser responsavel por processar essa linha de numero x.  


Cada processo vai abrir o arquivo usando fopen(), fazendo com que o mesmo arquivo seja aberto multiplas vezes, uma por processo.  

#### v2 - MPI_IRecv, MPI_ISend (sem MPI_Reduce)
Nessa versão tento corrigir as falhas da v1.

Optando por um modelo master-slave que implica no seguinte comportamento:  
O processo root (rank 0) faz a leitura do arquivo e distruibui as linhas para cada outro processo via MPI_Isend. Cada processo diferente do root conta as palavras da linha e envia o resulto para o processo root com MPI_Isend. No final, o processo root coleta os resultados com MPI_Irecv. Essa solução é sem o MPI_Reduce, pois já estou usando o MPI_Irecv no root pra receber as somas totais locais de cada subprocesso.