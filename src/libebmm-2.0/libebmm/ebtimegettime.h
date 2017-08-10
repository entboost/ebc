#ifndef EBTIMEGETTIME_H
#define EBTIMEGETTIME_H

#ifdef __MACH__
#include "mach_uses.h"
#endif
#ifdef WIN32
#include <CGCLib/CGCLib.h>
#include <Mmsystem.h>
#else
#include <time.h>
inline unsigned long timeGetTime()
{
    unsigned long uptime = 0;
    struct timespec on;
    if(clock_gettime(CLOCK_MONOTONIC, &on) == 0)
        uptime = on.tv_sec*1000 + on.tv_nsec/1000000;
    return uptime;
}
#endif

#endif // EBTIMEGETTIME_H
