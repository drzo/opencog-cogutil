/*
 * opencog/util/Logger.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008, 2010 OpenCog Foundation
 * Copyright (C) 2009, 2011, 2013 Linas Vepstas
 * All Rights Reserved
 *
 * Written by Andre Senna <senna@vettalabs.com>
 *            Gustavo Gama <gama@vettalabs.com>
 *            Linas Vepstas <linasvepstas@gmail.com>
 *            Joel Pitt <joel@opencog.org>
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

#include "Logger.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#else
#include <unistd.h>
#include <sys/time.h>
#endif

namespace opencog {

Logger::Logger(const std::string& fileName)
    : fileName(fileName),
      printToStdout(true),
      timestampEnabled(true),
      currentLevel(defaultLevel),
      backTraceLevel(defaultBackTraceLevel),
      f(nullptr)
{
    if (!fileName.empty()) {
        f = fopen(fileName.c_str(), "a");
        if (f == nullptr) {
            fprintf(stderr, "[ERROR] Unable to open log file \"%s\"\n", fileName.c_str());
            printToStdout = true;
        }
    }
}

Logger::~Logger()
{
    if (f != nullptr) fclose(f);
}

void Logger::setLevel(Level level) 
{
    currentLevel = level;
}

void Logger::setBackTraceLevel(Level level)
{
    backTraceLevel = level;
}

void Logger::setPrintToStdoutFlag(bool flag)
{
    printToStdout = flag;
}

void Logger::setTimestampFlag(bool flag)
{
    timestampEnabled = flag;
}

void Logger::setFilename(const std::string& s)
{
    if (f != nullptr) fclose(f);

    fileName = s;
    if (!fileName.empty()) {
        f = fopen(fileName.c_str(), "a");
        if (f == nullptr) {
            fprintf(stderr, "[ERROR] Unable to open log file \"%s\"\n", fileName.c_str());
            printToStdout = true;
        }
    }
}

void Logger::log(Level level, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logva(level, fmt, ap);
    va_end(ap);
}

void Logger::logva(Level level, const char *fmt, va_list args)
{
    if (level > currentLevel) return;

    std::lock_guard<std::mutex> lock(logMutex);

    char timestamp[64];
    if (timestampEnabled) {
        time_t t = time(nullptr);
        struct tm *tm = localtime(&t);
        strftime(timestamp, sizeof(timestamp), "[%Y-%m-%d %H:%M:%S] ", tm);
    } else {
        timestamp[0] = '\0';
    }

    char prefix[16];
    switch (level) {
    case ERROR:   strcpy(prefix, "[ERROR] "); break;
    case WARN:    strcpy(prefix, "[WARN] "); break;
    case INFO:    strcpy(prefix, "[INFO] "); break;
    case DEBUG:   strcpy(prefix, "[DEBUG] "); break;
    case FINE:    strcpy(prefix, "[FINE] "); break;
    default:      strcpy(prefix, "[NONE] "); break;
    }

    char buffer[16384];
    vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (printToStdout) {
        printf("%s%s%s\n", timestamp, prefix, buffer);
        fflush(stdout);
    }

    if (f) {
        fprintf(f, "%s%s%s\n", timestamp, prefix, buffer);
        fflush(f);
    }
}

void Logger::error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logva(ERROR, fmt, ap);
    va_end(ap);
}

void Logger::warn(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logva(WARN, fmt, ap);
    va_end(ap);
}

void Logger::info(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logva(INFO, fmt, ap);
    va_end(ap);
}

void Logger::debug(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logva(DEBUG, fmt, ap);
    va_end(ap);
}

void Logger::fine(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    logva(FINE, fmt, ap);
    va_end(ap);
}

Logger& Logger::getLogger()
{
    static Logger instance;
    return instance;
}

const char* Logger::getLevelString(const Level level)
{
    static const char* levelStrings[] = {
        "NONE",  "ERROR", "WARN", "INFO", "DEBUG", "FINE", "BAD_LEVEL"
    };
    return levelStrings[level];
}

Logger::Level Logger::getLevelFromString(const std::string& levelStr)
{
    if (levelStr == "NONE") return NONE;
    if (levelStr == "ERROR") return ERROR;
    if (levelStr == "WARN") return WARN;
    if (levelStr == "INFO") return INFO;
    if (levelStr == "DEBUG") return DEBUG;
    if (levelStr == "FINE") return FINE;
    return BAD_LEVEL;
}

void Logger::enableTimestamp() { timestampEnabled = true; }
void Logger::disableTimestamp() { timestampEnabled = false; }

} // namespace opencog
