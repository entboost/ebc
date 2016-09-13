

#ifdef WIN32
#include "config_win32.h"
#else
#include "config_unix.h"
#include <sys/time.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "timemonitor.h"

#ifdef _WIN32
static LARGE_INTEGER _tstart, _tend;
static LARGE_INTEGER freq;

void tstart(void)
{
	static int first = 1;

	if(first) {
		QueryPerformanceFrequency(&freq);
		first = 0;
	}
	QueryPerformanceCounter(&_tstart);
}
void tend(void)
{
	QueryPerformanceCounter(&_tend);
}

double tval()
{
	return ((double)_tend.QuadPart -
		(double)_tstart.QuadPart)/((double)freq.QuadPart);
}

#else
static struct timeval _tstart, _tend;
static struct timezone tz;

void tstart(void)
{
	gettimeofday(&_tstart, &tz);
}
void tend(void)
{
	gettimeofday(&_tend,&tz);
}

double tval()
{
	double t1, t2;

	t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec/(1000*1000);
	t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec/(1000*1000);
	return t2-t1;
}
#endif
