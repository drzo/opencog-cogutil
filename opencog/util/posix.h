#ifndef _OPENCOG_POSIX_H
#define _OPENCOG_POSIX_H

#ifdef _WIN32
#include <windows.h>
#include <sys/timeb.h>
#include <direct.h>
#include <io.h>

// Define POSIX-style functions for Windows
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define popen _popen
#define pclose _pclose
#define getcwd _getcwd
#define unlink _unlink
#define mkdir(path, mode) _mkdir(path)
#define chdir _chdir

// Define ISO646 operators for Windows
#ifndef __cplusplus
#define and &&
#define or ||
#define not !
#define xor ^
#endif

// Define POSIX-style types
typedef HANDLE process_id_t;

// Get current process ID
inline process_id_t get_pid()
{
    return GetCurrentProcess();
}

// Implement gettimeofday for Windows
struct timezone 
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

struct timeval 
{
    long tv_sec;
    long tv_usec;
};

inline int gettimeofday(struct timeval* tv, struct timezone* tz)
{
    if (tv)
    {
        FILETIME ft;
        unsigned __int64 tmpres = 0;
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        tmpres /= 10;  // Convert to microseconds
        tmpres -= 11644473600000000ULL; // Convert file time to unix epoch

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

#else
#include <unistd.h>
#include <sys/time.h>
typedef pid_t process_id_t;

inline process_id_t get_pid()
{
    return getpid();
}
#endif // _WIN32

#endif // _OPENCOG_POSIX_H 