/*
 * opencog/util/platform.h
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Moshe Looks <moshe@metacog.org>
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

#ifndef _OPENCOG_PLATFORM_H
#define _OPENCOG_PLATFORM_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef _WIN32
// Windows-specific includes and definitions
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <io.h>
#include <direct.h>

// Define POSIX-style functions for Windows
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define popen _popen
#define pclose _pclose
#define getcwd _getcwd
#define unlink _unlink
#define mkdir(path, mode) _mkdir(path)
#define chdir _chdir

// Define PATH_MAX for Windows
#ifndef PATH_MAX
#define PATH_MAX MAX_PATH
#endif

// Add ISO646 operators for Windows
#ifndef __cplusplus
#define and &&
#define or ||
#define not !
#define xor ^
#endif

#else
// POSIX/Unix includes
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#endif

// Common includes for both Windows and POSIX
#include <iso646.h>

namespace opencog
{
// Platform-independent type definitions and functions
#ifdef _WIN32
typedef HANDLE process_id_t;
#else
typedef pid_t process_id_t;
#endif

// Get current process ID
inline process_id_t get_pid()
{
#ifdef _WIN32
    return GetCurrentProcess();
#else
    return getpid();
#endif
}

} // namespace opencog

#endif // _OPENCOG_PLATFORM_H
