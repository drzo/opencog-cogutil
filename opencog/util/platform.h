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

#ifdef WIN32
#include <windows.h>
#include <process.h>
#include <direct.h>
#include <io.h>
typedef int pid_t;
#define getpid _getpid
#define getcwd _getcwd
#define chdir _chdir
#define access _access
#define F_OK 0
#else
#include <unistd.h>
#include <sys/time.h>
#endif

#include <string>

namespace opencog
{
    typedef pid_t process_id_t;

    inline process_id_t get_pid()
    {
#ifdef WIN32
        return _getpid();
#else
        return ::getpid();
#endif
    }
}

#endif // _OPENCOG_PLATFORM_H
