#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "common/timer.hpp"

#define WORKLOAD_PATH "file1.dat"
#define RUNTIME_SEC 60

struct ItemSentence 
{
    string line;
    double timestamp;
};

struct ItemWord 
{
    string word;
    unsigned long count;
    double timestamp;
};

void WordCount();
void Splitter(ItemSentence);
void Counter(ItemWord);
void Sink(ItemWord);

unordered_map<string, int> word_frequency_map;
bool is_running = true;
int rate = 0;
double latency_acc = 0;
unsigned long source_bytes = 0;
unsigned long source_generated = 0;
unsigned long splitter_bytes = 0;
unsigned long splitter_lineCount = 0;
unsigned long splitter_wordCount = 0;
unsigned long counter_bytes = 0;
unsigned long counter_words = 0;
unsigned long sink_bytes = 0;
unsigned long sink_words = 0;

void WordCount() 
{
    try 
    {
        unsigned long id = 0;
        double epoch = current_time();
        ifstream file(WORKLOAD_PATH);
        string line;
        while ((current_time() - epoch < RUNTIME_SEC * 1e9) && is_running)
        {
            if (std::getline(file, line, '\n')) 
            {
                if (line.empty()) 
                {
                    continue;
                }
                source_bytes += line.size();
                source_generated++;
                Splitter(ItemSentence(line, current_time()));
                id++;

                if (rate != 0) 
                {
                    long delay_nsec = (long) ((1.0f / rate) * 1e9);
                    double start_time = current_time();
                    bool end = false;
                    while (!end) 
                    {
                        double end_time = current_time();
                        end = (end_time - start_time) >= delay_nsec;
                    }
                }
            }
            else
            {
                file.clear();
                file.seekg(0, ios::beg);
            }
        }
        file.close();
        is_running = false;
    }
    catch (const exception& e) 
    {
        printf("%s\n", e.what());
    }
}

void Splitter(ItemSentence input_item)
{
    if (!input_item.line.empty())
    {
        splitter_bytes += input_item.line.size();
        splitter_lineCount++;

        string word;
        istringstream stream(input_item.line);
        while (getline(stream, word, ' ')) {
            splitter_wordCount++;
            Counter(ItemWord(move(word), 1, input_item.timestamp));
        }
    }
}

void Counter(ItemWord input_item)
{
    if (!input_item.word.empty())
    {
        counter_bytes += input_item.word.size();
        counter_words++;
        word_frequency_map[input_item.word]++;
    }
    Sink(ItemWord(input_item.word, word_frequency_map[input_item.word], input_item.timestamp));
}

void Sink(ItemWord input_item)
{
    if (!input_item.word.empty())
    {
        sink_bytes += input_item.word.size();
        sink_words++;
        latency_acc += (current_time() - input_item.timestamp) / 1e03;
    }
}

int main(int argc, char **argv)
{
    WordCount();
    printf("Average latency: %f ms/word\n", latency_acc / sink_words);
    printf("Average throughput: %f mb/sec\n", (sink_bytes / 1e06) / RUNTIME_SEC);
}