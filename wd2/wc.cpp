#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <bits/stdc++.h>
#include <mpi.h>

#define NUM_LINES 4

using namespace std;
void printWords(vector<string> words);

void word_count(string path, string globalconcat)
{
    vector<string> words;
    unordered_map<string, int> dict;
    fstream file;
    int zero=0;
    int count=0;
    string w;
    file.open(path, ios::in);
    if (file.is_open())
    {
        puts("file is open.");
        string word;
        while(getline(file, word, ' '))
        {
            //std::cout << word << std::endl;
            words.push_back(word);
            count++;
        }
        file.close();
    }

    //printWords(words);
    printf("Contagem: %d\n", count);

    while(!words.empty())
    {
        w = words.back();
        words.pop_back();
        dict[w]++;
        //std::cout << dict[w] << std::endl;
    }
}

void printWords(const std::vector<std::string> words)
{
    for(int i = 0; i < words.size(); i++)
        std::cout << words[i] << std::endl;
}

int main(int argc, char *argv[])
{
    string path = "file1.dat";

    MPI_Init(NULL, NULL);

    int words_per_proc = 10

    int rank;
    int size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,  &size);

    string globalconcat = "";
    
    word_count(path, globalconcat);
    return 0;
}