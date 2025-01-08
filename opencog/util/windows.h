#ifndef _OPENCOG_WINDOWS_H
#define _OPENCOG_WINDOWS_H

#ifdef WIN32

#include <cstdint>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>

namespace opencog
{
    struct timeval {
        int64_t tv_sec;       /* seconds */
        int64_t tv_usec;      /* microseconds */
    };

    struct timezone {
        int tz_minuteswest;     /* minutes west of Greenwich */
        int tz_dsttime;         /* type of DST correction */
    };

    inline int gettimeofday(struct timeval* tv, struct timezone* tz)
    {
        if (tv != NULL)
        {
            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);
            uint64_t tt = filetime_to_unix_epoch(ft);
            tv->tv_sec = tt / 10000000;
            tv->tv_usec = (tt % 10000000) / 10;
        }
        return 0;
    }

    inline uint64_t filetime_to_unix_epoch(const FILETIME& ft)
    {
        uint64_t tt = ft.dwHighDateTime;
        tt <<= 32;
        tt |= ft.dwLowDateTime;
        tt -= 116444736000000000ULL;
        return tt;
    }
}

#endif // WIN32

#endif // _OPENCOG_WINDOWS_H 