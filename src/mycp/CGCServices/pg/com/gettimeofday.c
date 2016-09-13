
#ifdef WIN32
#include "config_win32.h"
#else
#include "config_unix.h"
#endif

#include "gettimeofday.h"

#if defined(NEED_GETTIMEOFDAY) && defined(WIN32)

int gettimeofday(struct timeval *tp, void *tz)
{
	struct _timeb timebuffer;   

	_ftime( &timebuffer );
	tp->tv_sec  = timebuffer.time;
	tp->tv_usec = timebuffer.millitm * 1000;
	return 0;
}

#endif



