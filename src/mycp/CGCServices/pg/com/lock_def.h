
#ifndef __LOCK_DEF_H_20060109__
#define __LOCK_DEF_H_20060109__

#ifndef WIN32
#include <pthread.h>
#else
#if defined(USE_PTHREAD)
#include <pthread.h>
#endif

#endif



#if defined(WIN32) && !defined (USE_PTHREAD)                             
#define LOCK_INIT(a)     InitializeCriticalSection(a)                
#define LOCK(a)          EnterCriticalSection(a)
#define UNLOCK(a)        LeaveCriticalSection(a)
#define LOCK_DESTROY(a)  DeleteCriticalSection (a)
#define SLEEPX(a)        Sleep(a)
#define LOCK_T	CRITICAL_SECTION
#define THREAD_T HANDLE

#else    
#define LOCK_INIT(a)     do { pthread_mutexattr_t    attr; \
							 pthread_mutexattr_init(&attr); \
							 pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP); \
							 pthread_mutex_init(a, &attr);\
							 pthread_mutexattr_destroy(&attr); }while(0)                                        
#define LOCK(a)          pthread_mutex_lock(a)
#define UNLOCK(a)        pthread_mutex_unlock(a)
#define LOCK_DESTROY(a)  pthread_mutex_destroy(a)
#define SLEEPX(a)        usleep(a*1000)
#define LOCK_T	pthread_mutex_t
#define THREAD_T pthread_t
#endif   

#endif



