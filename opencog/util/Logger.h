/*
 * opencog/util/Logger.h
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008 by OpenCog Foundation
 * All Rights Reserved
 *
 * Written by Andre Senna <senna@vettalabs.com>
 *            Gustavo Gama <gama@vettalabs.com>
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

#ifndef _OPENCOG_LOGGER_H
#define _OPENCOG_LOGGER_H

#include <cstdarg>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

#include "platform.h"

#ifdef _MSC_VER
#pragma warning(disable : 4996)  // Disable deprecation warnings
#endif

namespace opencog
{

class Logger
{
public:
    enum LogLevel {
        NONE = 0,    // No messages
        ERROR = 1,   // Error messages
        WARN = 2,    // Warning messages
        INFO = 3,    // Information messages
        DEBUG = 4,   // Debug messages
        FINE = 5     // Fine debug messages
    };

    static const char* getLevelString(LogLevel level);

private:
    std::string fileName;
    bool timestampEnabled;
    bool threadIdEnabled;
    bool printToStdout;
    bool printLevel;
    bool syncEnabled;
    LogLevel currentLevel;
    LogLevel previousLevel;
    LogLevel backTraceLevel;
    std::mutex logMutex;

    void write_msg(const std::string &msg);

public:
    explicit Logger(const std::string &fileName = "opencog.log");
    virtual ~Logger();

    void setLevel(LogLevel level) { currentLevel = level; }
    void setBackTraceLevel(LogLevel level) { backTraceLevel = level; }
    void setPreviousLevel() { currentLevel = previousLevel; }
    
    LogLevel getLevel() const { return currentLevel; }
    LogLevel getBackTraceLevel() const { return backTraceLevel; }

    void setTimestampFlag(bool flag) { timestampEnabled = flag; }
    void setThreadIdFlag(bool flag) { threadIdEnabled = flag; }
    void setPrintToStdoutFlag(bool flag) { printToStdout = flag; }
    void setPrintLevelFlag(bool flag) { printLevel = flag; }
    void setSyncFlag(bool flag) { syncEnabled = flag; }

    bool getTimestampFlag() const { return timestampEnabled; }
    bool getThreadIdFlag() const { return threadIdEnabled; }
    bool getPrintToStdoutFlag() const { return printToStdout; }
    bool getPrintLevelFlag() const { return printLevel; }
    bool getSyncFlag() const { return syncEnabled; }

    void error(const std::string &txt) { log(ERROR, txt); }
    void warn(const std::string &txt) { log(WARN, txt); }
    void info(const std::string &txt) { log(INFO, txt); }
    void debug(const std::string &txt) { log(DEBUG, txt); }
    void fine(const std::string &txt) { log(FINE, txt); }

    void error(const char *fmt, ...);
    void warn(const char *fmt, ...);
    void info(const char *fmt, ...);
    void debug(const char *fmt, ...);
    void fine(const char *fmt, ...);

    void log(LogLevel level, const char *fmt, ...);
    void logva(LogLevel level, const char *fmt, va_list args);
    void log(LogLevel level, const std::string &txt);

    std::string getFileName() const { return fileName; }
};

// Global logger instance
Logger& logger();

} // namespace opencog

#endif // _OPENCOG_LOGGER_H
