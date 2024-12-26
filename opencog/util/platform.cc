/*
 * opencog/util/platform.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008 by OpenCog Foundation
 * All Rights Reserved
 *
 * Written by Gustavo Gama <moshe@metacog.org>
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

#include "platform.h"
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

namespace opencog {

const char* getUserName() {
    const char* username = getenv("LOGNAME");
    if (username == NULL)
        username = getenv("USER");
    if (username == NULL)
        username = "unknown_user";
    return username;
}

std::string get_exe_name()
{
#ifdef _MSC_VER
    char path[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string s = path;
    return s.substr(s.find_last_of("\\/") + 1);
#else
    char path[PATH_MAX];
    ssize_t size = readlink("/proc/self/exe", path, PATH_MAX);
    path[size] = '\0';
    std::string s = path;
    return s.substr(s.find_last_of('/') + 1);
#endif
}

std::string get_exe_dir()
{
#ifdef _MSC_VER
    char path[MAX_PATH] = { 0 };
    GetModuleFileNameA(NULL, path, MAX_PATH);
    std::string s = path;
    return s.substr(0, s.find_last_of("\\/"));
#else
    char path[PATH_MAX];
    ssize_t size = readlink("/proc/self/exe", path, PATH_MAX);
    path[size] = '\0';
    std::string s = path;
    return s.substr(0, s.find_last_of('/'));
#endif
}

std::string get_current_dir()
{
    char path[PATH_MAX];
    getcwd(path, PATH_MAX);
    return std::string(path);
}

bool create_directory(const std::string& dir)
{
    try {
        return fs::create_directories(dir);
    } catch (...) {
        return false;
    }
}

bool exists(const std::string& path)
{
    try {
        return fs::exists(path);
    } catch (...) {
        return false;
    }
}

void set_current_dir(const std::string& dir)
{
    chdir(dir.c_str());
}

} // ~namespace opencog

// ===========================================

#ifdef __APPLE__

#include <sys/timeb.h>
#include <math.h>

#ifndef HAVE_STRTOK_R
#define HAVE_STRTOK_R 1

char* __strtok_r(char *s1, const char *s2, char **lasts)
{
    char *ret;

    if (s1 == NULL)
        s1 = *lasts;
    while (*s1 && strchr(s2, *s1))
        ++s1;
    if (*s1 == '\0')
        return NULL;
    ret = s1;
    while (*s1 && !strchr(s2, *s1))
        ++s1;
    if (*s1)
        *s1++ = '\0';
    *lasts = s1;
    return ret;
}

#endif /* HAVE_STRTOK_R */
#endif /* __APPLE__ */

// ======================================================
#ifdef WIN32_NOT_UNIX

#include <sys/timeb.h>
#include <winsock2.h>
#include <process.h>
#include <math.h>
#include <io.h>

int round(float x)
{
    return ((x -(int)(x)) < 0.5 ) ? (int)x : (int)x + 1;
}

int gettimeofday(struct timeval* tp, void* tzp)
{
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    tp->tv_sec = (long) timebuffer.time;
    tp->tv_usec = timebuffer.millitm * 1000;
    /* 0 indicates that the call succeeded. */
    return 0;
}

void usleep(unsigned useconds)
{
    // Sleep is in milliseconds
    // If 0 is passed to Sleep()
    // It skips rest of thread scheduled time
    // This is the best achievable with Millisecond
    // resolution
    Sleep((int)(useconds / 1000));
}

unsigned sleep(unsigned seconds)
{
    Sleep(seconds * 1000);
    return 0;
}

#ifndef HAVE_STRTOK_R
#define HAVE_STRTOK_R 1

char* __strtok_r(char *s1, const char *s2, char **lasts)
{
    char *ret;

    if (s1 == NULL)
        s1 = *lasts;
    while (*s1 && strchr(s2, *s1))
        ++s1;
    if (*s1 == '\0')
        return NULL;
    ret = s1;
    while (*s1 && !strchr(s2, *s1))
        ++s1;
    if (*s1)
        *s1++ = '\0';
    *lasts = s1;
    return ret;
}

#endif /* HAVE_STRTOK_R */

int __getpid(void)
{
    return _getpid();
}

double rint(double nr)
{
    double f = floor(nr);
    double c = ceil(nr);
    return (((c -nr) >= (nr - f)) ? f : c);
}

int __dup2(int fd1, int fd2)
{
    return _dup2(fd1, fd2);
}

unsigned long long atoll(const char *str)
{
    unsigned long long la = 0;
    sscanf(str, "%Lu", &la);
    return la;
}

#endif // WIN32_NOT_UNIX

// ==========================================================

#include <stdlib.h>
#include <unistd.h>   // for sbrk(), sysconf()

// Return memory usage per sbrk system call.
size_t opencog::getMemUsage()
{
    static void *old_sbrk = 0;
    void *p = sbrk(0);
    if (old_sbrk == 0 || old_sbrk > p)
    {
        old_sbrk = p;
        return 0;
    }
    size_t diff = (size_t)p - (size_t)old_sbrk;
    return diff;
}

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <sys/types.h>
#include <pthread.h>

uint64_t opencog::getTotalRAM()
{
   int mib[2];
   uint64_t physmem;
   size_t len;

   mib[0] = CTL_HW;
   mib[1] = HW_MEMSIZE;
   len = sizeof(physmem);
   sysctl(mib, 2, &physmem, &len, NULL, 0);
   return physmem;
}

uint64_t opencog::getFreeRAM()
{
    return getTotalRAM() - getMemUsage();
}

void opencog::set_thread_name(const char* name)
{
    pthread_setname_np(name);
}

#else // __APPLE__

// If not Apple, then Linux.
#include <sys/sysinfo.h>
#include <sys/prctl.h>

uint64_t opencog::getTotalRAM()
{
    // return getpagesize() * get_phys_pages();
    return getpagesize() * sysconf(_SC_PHYS_PAGES);
}

uint64_t opencog::getFreeRAM()
{
    // return getpagesize() * get_avphys_pages();
    return getpagesize() * sysconf(_SC_AVPHYS_PAGES);
}

void opencog::set_thread_name(const char* name)
{
    prctl(PR_SET_NAME, name, 0, 0, 0);
}
#endif // __APPLE__

#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

#include <cstdlib>
#include <string>

namespace opencog {

std::string get_current_dir() {
    char buffer[PATH_MAX];
#ifdef WIN32
    if (_getcwd(buffer, PATH_MAX) == nullptr)
#else
    if (getcwd(buffer, PATH_MAX) == nullptr)
#endif
        return "";
    return std::string(buffer);
}

bool change_directory(const std::string& directory) {
#ifdef WIN32
    return _chdir(directory.c_str()) == 0;
#else
    return chdir(directory.c_str()) == 0;
#endif
}

bool file_exists(const std::string& filename) {
#ifdef WIN32
    return _access(filename.c_str(), F_OK) == 0;
#else
    return access(filename.c_str(), F_OK) == 0;
#endif
}

std::string get_environment_variable(const std::string& var_name) {
#ifdef WIN32
    char buffer[32767]; // Maximum size for Windows environment variables
    DWORD result = GetEnvironmentVariableA(var_name.c_str(), buffer, sizeof(buffer));
    if (result == 0 || result > sizeof(buffer))
        return "";
    return std::string(buffer);
#else
    const char* value = getenv(var_name.c_str());
    return value ? std::string(value) : "";
#endif
}

bool set_environment_variable(const std::string& var_name, const std::string& value) {
#ifdef WIN32
    return SetEnvironmentVariableA(var_name.c_str(), value.c_str()) != 0;
#else
    return setenv(var_name.c_str(), value.c_str(), 1) == 0;
#endif
}

} // namespace opencog
