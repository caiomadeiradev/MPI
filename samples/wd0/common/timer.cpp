#include "timer.hpp"

double current_time(void)
{
	return duration_cast<nanoseconds>(steady_clock::now().time_since_epoch()).count();
}

double start[64], elapsed[64];

void timer_clear(int n)
{
	elapsed[n] = 0.0;
}

void timer_start(int n)
{
	start[n] = current_time();
}

void timer_stop(int n)
{
	double t, now;
	now = current_time();
	t = now - start[n];
	elapsed[n] += t;

}

double timer_read(int n)
{
	return(elapsed[n]);
}