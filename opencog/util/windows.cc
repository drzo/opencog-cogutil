#ifdef _WIN32

#include <windows.h>
#include <time.h>
#include "windows.h"

int gettimeofday(struct timeval* tv, struct timezone* tz)
{
    if (tv)
    {
        FILETIME ft;
        unsigned __int64 tmpres = 0;
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        tmpres /= 10;  // convert into microseconds
        tmpres -= 11644473600000000ULL;  // convert file time to unix epoch

        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (tz)
    {
        TIME_ZONE_INFORMATION TimeZoneInfo;
        GetTimeZoneInformation(&TimeZoneInfo);

        tz->tz_minuteswest = TimeZoneInfo.Bias;
        tz->tz_dsttime = 0;
    }

    return 0;
}

#endif // _WIN32 