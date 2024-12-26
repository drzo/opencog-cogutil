/*
 * opencog/util/files.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
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

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include "files.h"
#include "platform.h"
#include "Logger.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/filesystem.hpp>

#ifdef WIN32
#include <direct.h>
#include <windows.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#define mkdir _mkdir
#define PATH_MAX MAX_PATH
#else
#include <unistd.h>
#ifndef PATH_MAX
#define PATH_MAX 1024
#endif
#endif

#define USER_FLAG "$USER"

/**
 * The file paths here have to be able to find both shared libraries
 * (dynamically loadable libraries) as well as scheme modules and files.
 *
 * The basic search algo is that anything in the user's directory is
 * explored first, before anything in the system directories.
 *
 * The user can set the environmental OPENCOG_MODULE_PATHS to add more
 * module paths after compile time.
 */
static const std::vector<std::string> paths =
{
#ifndef WIN32
    // XXX FIXME Searching the current path is a security breach just
    // waiting to happen, but the current OpenCog cogserver and Config
    // and unit-test-case design more or less demands this. The unit
    // tests and/or the config infrastructure would need to be overhauled.
    //
    "./",
    "../",
    "../../",
    "../../../",
    "../../../../",   // some unit tests need this
#endif // !WIN32
    CMAKE_INSTALL_PREFIX "/lib",
    CMAKE_INSTALL_PREFIX "/share",
    DATADIR,         // this too is an install dir
#ifndef WIN32
    "/usr/local/lib64/",  // lib64 is used on CentOS systems.
    "/usr/local/lib/",    // search local first, then system.
    "/usr/local/share/",  // search local first, then system.
    "/usr/lib64/",
    "/usr/lib/",
    "/usr/share/",
    "/opt/",
    "/",
#endif // !WIN32
};
const std::vector<std::string> opencog::DEFAULT_MODULE_PATHS = paths;

std::vector<std::string> opencog::get_module_paths()
{
    std::vector<std::string> results = opencog::DEFAULT_MODULE_PATHS;
    if (const char* env_p = std::getenv("OPENCOG_MODULE_PATHS"))
    {
        std::vector<std::string> split_paths;
        std::string paths(env_p);
        boost::split(split_paths, paths, boost::is_any_of(":"));
        results.insert(results.end(), split_paths.begin(), split_paths.end());
    }
    return results;
}

bool opencog::file_exists(const char* filename)
{
#ifdef WIN32
    return PathFileExistsA(filename) == TRUE;
#else
    std::fstream dumpFile(filename, std::ios::in);
    dumpFile.close();
    if (dumpFile.fail()) {
        dumpFile.clear(std::ios_base::failbit);
        return false;
    }
    return true;
#endif
}

bool opencog::exists(const char *fname)
{
#ifdef WIN32
    return PathFileExistsA(fname) == TRUE;
#else
    FILE* f = fopen(fname, "rb");
    if (!f)
        return false;
    fclose(f);
    return true;
#endif
}

void opencog::expand_path(std::string& path)
{
#ifdef WIN32
    // Convert forward slashes to backslashes for Windows
    std::replace(path.begin(), path.end(), '/', '\\');
#endif

    size_t user_index = path.find(USER_FLAG, 0);
    if (user_index != std::string::npos) {
        const char* username = getUserName();
        path.replace(user_index, strlen(USER_FLAG), username);
    }
}

bool opencog::create_directory(const char* directory)
{
#ifdef WIN32
    std::string dir_path(directory);
    std::replace(dir_path.begin(), dir_path.end(), '/', '\\');
    
    // Create all intermediate directories
    for (size_t pos = 0; pos < dir_path.length(); pos++) {
        if (dir_path[pos] == '\\') {
            std::string subdir = dir_path.substr(0, pos);
            if (!subdir.empty()) {
                _mkdir(subdir.c_str());
            }
        }
    }
    return (_mkdir(dir_path.c_str()) == 0 || errno == EEXIST);
#else
    if (mkdir(directory, S_IRWXU | S_IRWXG | S_IRWXO) == 0 || errno == EEXIST) {
        return true;
    }
    return false;
#endif
}

bool opencog::append_file_content(const char* filename, std::string &s)
{
    std::ifstream in(filename);
    if (!in.is_open())
        return false;

    char c;
    std::string str;
    while (in.get(c))
        str += c;

    if (!in.eof())
        return false;

    s = str;
    return true;
}

bool opencog::load_text_file(const std::string &fname, std::string& dest)
{
    FILE *f = fopen(fname.c_str(), "rt");
    if (f == NULL) {
        puts("File not found.");
        return false;
    }
    fseek(f, 0L, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0L, SEEK_SET);

    char *buf = new char[fsize+2];
    long bptr = 0;

    while (!feof(f))
        buf[bptr++] = getc(f);
    buf[bptr] = '\0';

    fclose(f);

    dest = buf;

    delete[] buf;

    return true;
}

std::string opencog::get_exe_name()
{
#ifdef WIN32
    static char buf[PATH_MAX];
    GetModuleFileName(NULL, buf, PATH_MAX);
    return std::string(buf);
#else
    static char buf[PATH_MAX];
    int rslt = readlink("/proc/self/exe", buf, PATH_MAX);

    if (rslt < 0 || rslt >= PATH_MAX) {
        return std::string();
    }

    buf[rslt] = '\0';
    return std::string(buf);
#endif
}

std::string opencog::get_exe_dir()
{
    std::string exeName = get_exe_name();
#ifdef WIN32
    size_t pos = exeName.find_last_of("\\/");
#else
    size_t pos = exeName.find_last_of('/');
#endif
    return (pos != std::string::npos) ? exeName.substr(0, pos + 1) : "";
}

// Add Windows-specific path normalization
std::string opencog::normalize_path(const std::string& path)
{
#ifdef WIN32
    std::string normalized = path;
    std::replace(normalized.begin(), normalized.end(), '/', '\\');
    return normalized;
#else
    return path;
#endif
}
