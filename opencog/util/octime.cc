/*
 * opencog/util/octime.cc
 *
 * Copyright (C) 2011 OpenCog Foundation
 * All Rights Reserved
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdlib.h>
#include <string>
#include <time.h>
#include <windows.h>
#ifdef WIN32_NOT_UNIX
#include <winsock2.h>
#else
#include <sys/time.h>
#endif

#include <opencog/util/octime.h>
#include <opencog/util/exceptions.h>
#include <opencog/util/oc_assert.h>

using namespace opencog;

namespace opencog {

/**
 * Time used as reference to set/get timestamps over the code
 * Used as a handy-dandy but crude profiling utility
 */
static timeval referenceTime;
static bool referenceTimeInitialized = false;

inline uint64_t filetime_to_unix_epoch(const FILETIME& ft)
{
    uint64_t tt = ft.dwHighDateTime;
    tt <<= 32;
    tt |= ft.dwLowDateTime;
    tt -= 116444736000000000ULL;
    return tt;
}

int gettimeofday(struct timeval* tp, void* tzp)
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    uint64_t tt = filetime_to_unix_epoch(ft);
    tt /= 10;
    tp->tv_sec = (long)(tt / 1000000UL);
    tp->tv_usec = (long)(tt % 1000000UL);
    return 0;
}

void init_reference_time()
{
    gettimeofday(&referenceTime, NULL);
    referenceTimeInitialized = true;
}

unsigned long get_elapsed_millis()
{
    OC_ASSERT(referenceTimeInitialized,
            "utils - referenceTimeInitialized should have been initialized.");
    timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return (currentTime.tv_sec - referenceTime.tv_sec) * 1000 + 
           (currentTime.tv_usec - referenceTime.tv_usec) / 1000;
}

};

