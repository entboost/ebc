#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer_call.h"
#include <sys/time.h>

struct timer_call {
    struct timer_call         *next;
    TimerFunc                  func;
    void                      *u;
    int                        repeat;
    long                       interval;
    struct timeval             next_round;
};
    

static struct timer_call *timer_head = 0;
static pthread_mutex_t    timer_lock = PTHREAD_MUTEX_INITIALIZER;

static long cmp_time(struct timeval *a, struct timeval *b)
{
    return (a->tv_sec) * 1000 + (a->tv_usec) / 1000 - 
	(b->tv_sec) * 1000 - (b->tv_usec) / 1000;
}

/*interval caculated as millisecond 
 *and tv is an out parameter
 */
static void cacl_next_round(struct timeval *tv, struct timeval *cur, long interval)
{
    long microseconds;
    
    microseconds = cur->tv_usec + interval * 1000;

    tv->tv_sec = cur->tv_sec + microseconds / 1000000;
    tv->tv_usec = cur->tv_usec + microseconds / 1000000;
}

static void _insert_timer_call(struct timer_call *tc)
{
    tc->next   = timer_head;
    timer_head = tc;
}

static void insert_timer_call(struct timer_call *tc)
{
    pthread_mutex_lock(&timer_lock);
    _insert_timer_call(tc);
    pthread_mutex_unlock(&timer_lock);
}

void register_timer_call(long millisecond, int repeat, TimerFunc func, void *arg)
{


}

void timeout_call (long millisecond, int repeat, TimerFunc func, void *arg)
{
    struct timer_call *tc;
    struct timeval     tv;
    if(millisecond < 0)
	return;
    if(millisecond == 0) {
	func(arg);
	return;
    }
    
    tc = (struct timer_call*)malloc(sizeof(struct timer_call));
    tc->next     = 0;
    tc->func     = func;
    tc->u        = arg;
    tc->interval = millisecond;
    tc->repeat   = repeat;
    gettimeofday(&tv, 0);
    cacl_next_round(&tc->next_round, &tv, millisecond);
    insert_timer_call(tc);
} 

void schedule_timeout_call() 
{
    struct timer_call *pos, *prev, *next, *new_list;
    struct timeval     tv;
    long               diff;

    pthread_mutex_lock(&timer_lock);
    pos  = timer_head;
    prev = 0;
    gettimeofday(&tv, 0);
    
    while (pos) {
      next = pos->next;
      diff = cmp_time(&pos->next_round, &tv);
      
      if (diff < 0) {
	pos->func(pos->u);
#ifdef DEBUG_TIMER
	printf("shedule run at %d:%d\n", tv.tv_sec, tv.tv_usec);
#endif
	if(pos->repeat) {
	  cacl_next_round(&pos->next_round, &tv, pos->interval);
	} else {
	  if(prev) {
	    prev->next = next;
	  } else {
	    timer_head = next;
	  }
	  free(pos);
	  continue;
	}
      } 
      pos  = next;
      prev = pos;
    }
    
    pthread_mutex_unlock(&timer_lock);
}

void schedule_timer() 
{
    struct timer_call *pos, *prev, *next, *new_list;
    struct timeval     tv;
    long               diff;

    while (1) {
	pthread_mutex_lock(&timer_lock);
	pos  = timer_head;
	prev = 0;
	gettimeofday(&tv, 0);

	while (pos) {
	    next = pos->next;
	    diff = cmp_time(&pos->next_round, &tv);

	    if (diff < 0) {
		pos->func(pos->u);
		#ifdef DEBUG_TIMER
		printf("shedule run at %d:%d\n", tv.tv_sec, tv.tv_usec);
		#endif
		if(pos->repeat) {
		    cacl_next_round(&pos->next_round, &tv, pos->interval);
		} else {
		    if(prev) {
			prev->next = next;
		    } else {
			timer_head = next;
		    }
		    free(pos);
		    continue;
		}
	    } 
	    pos  = next;
	    prev = pos;
	}
	
	pthread_mutex_unlock(&timer_lock);
	usleep(1000);
    }
}
