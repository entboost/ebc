
#ifndef __TIMER_CALL_H__
#define __TIMER_CALL_H__

typedef void (*TimerFunc) (void *arg);
#ifdef __cplusplus 
extern "C" {
#endif

    void timeout_call (long millisecond, int repeat, TimerFunc func, void *arg);
    void schedule_timer();
    void schedule_timeout_call() ;

#ifdef __cplusplus
}
#endif

#endif /* __TIMER_CALL_H__ */

