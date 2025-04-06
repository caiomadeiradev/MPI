#ifndef TIMER_H
#define TIMER_H

#include <chrono>

using namespace std;
using namespace chrono;

double current_time(void);
void timer_clear(int n);
void timer_start(int n);
void timer_stop(int n);
double timer_read(int n);

#endif