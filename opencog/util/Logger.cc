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

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

#include "Logger.h"
#include "platform.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996)  // Disable deprecation warnings for fopen
#endif

using namespace opencog;

// Messages greater than this will be truncated
#define MAX_PRINTF_STYLE_MESSAGE_SIZE (1024)

const char* Logger::getLevelString(Level level)
{
    switch(level) {
    case Level::NONE:  return "NONE";
    case Level::ERROR: return "ERROR";
    case Level::WARN:  return "WARN";
    case Level::INFO:  return "INFO";
    case Level::DEBUG: return "DEBUG";
    case Level::FINE:  return "FINE";
    default:          return "BAD LEVEL";
    }
}

Logger::Logger(const std::string &fileName)
{
    this->fileName.assign(fileName);
    this->currentLevel = Level::INFO;
    this->backTraceLevel = Level::ERROR;
    this->timestampEnabled = true;
    this->threadIdEnabled = true;
    this->printToStdout = false;
    this->printLevel = true;
    this->syncEnabled = false;
}

Logger::~Logger() = default;

void Logger::setFilename(const std::string& s)
{
    fileName.assign(s);
}

void Logger::setTimestampFlag(bool flag)
{
    timestampEnabled = flag;
}

void Logger::setThreadIdFlag(bool flag)
{
    threadIdEnabled = flag;
}

void Logger::setPrintToStdoutFlag(bool flag)
{
    printToStdout = flag;
}

void Logger::setPrintLevelFlag(bool flag)
{
    printLevel = flag;
}

void Logger::setSyncEnabled(bool flag)
{
    syncEnabled = flag;
}

void Logger::error(const char *fmt, ...)
{
    va_list args; va_start(args, fmt);
    logva(Level::ERROR, fmt, args);
    va_end(args);
}

void Logger::warn(const char *fmt, ...)
{
    va_list args; va_start(args, fmt);
    logva(Level::WARN, fmt, args);
    va_end(args);
}

void Logger::info(const char *fmt, ...)
{
    va_list args; va_start(args, fmt);
    logva(Level::INFO, fmt, args);
    va_end(args);
}

void Logger::debug(const char *fmt, ...)
{
    va_list args; va_start(args, fmt);
    logva(Level::DEBUG, fmt, args);
    va_end(args);
}

void Logger::fine(const char *fmt, ...)
{
    va_list args; va_start(args, fmt);
    logva(Level::FINE, fmt, args);
    va_end(args);
}

void Logger::logva(Level level, const char *fmt, va_list args)
{
    if (level > currentLevel) return;

    char buffer[MAX_PRINTF_STYLE_MESSAGE_SIZE];
#ifdef _MSC_VER
    vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, fmt, args);
#else
    vsnprintf(buffer, sizeof(buffer), fmt, args);
#endif
    std::string msg = buffer;
    log(level, msg);
}

void Logger::log(Level level, const std::string &txt)
{
    if (level > currentLevel) return;

    std::ostringstream oss;
    if (timestampEnabled) {
        time_t timestamp;
        time(&timestamp);
        char timestamp_buffer[64];
#ifdef _MSC_VER
        tm timeinfo;
        localtime_s(&timeinfo, &timestamp);
        strftime(timestamp_buffer, sizeof(timestamp_buffer),
                "%Y-%m-%d %H:%M:%S", &timeinfo);
#else
        strftime(timestamp_buffer, sizeof(timestamp_buffer),
                "%Y-%m-%d %H:%M:%S", localtime(&timestamp));
#endif
        oss << "[" << timestamp_buffer << "] ";
    }

    if (threadIdEnabled)
        oss << "[Thread-" << std::this_thread::get_id() << "] ";

    if (printLevel)
        oss << "[" << getLevelString(level) << "] ";

    oss << txt << std::endl;

    if (syncEnabled) {
        std::lock_guard<std::mutex> lock(this->lock);
        write_msg(oss.str());
    } else {
        write_msg(oss.str());
    }
}

void Logger::write_msg(const std::string &msg)
{
    if (printToStdout) {
        std::cout << msg;
        std::cout.flush();
    }

    if (!fileName.empty()) {
#ifdef _MSC_VER
        FILE* f = nullptr;
        if (fopen_s(&f, fileName.c_str(), "a") == 0) {
            if (f) {
                fprintf_s(f, "%s", msg.c_str());
                fclose(f);
            }
        }
#else
        std::FILE* f = fopen(fileName.c_str(), "a");
        if (f) {
            fprintf(f, "%s", msg.c_str());
            fclose(f);
        }
#endif
    }
}

void Logger::flush()
{
    if (printToStdout)
        std::cout.flush();
}
