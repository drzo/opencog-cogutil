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
#include <string>
#include <functional>
#include <iostream>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#define getcwd _getcwd
#define chdir _chdir
#endif

namespace opencog
{

class Logger
{
public:
    // Warning: if you change the levels don't forget to update
    // levelStrings[] in Logger.cc
    enum Level {
        NONE,
        ERROR,
        WARN,
        INFO,
        DEBUG,
        FINE,
        BAD_LEVEL
    };

    static const Level defaultLevel = Level::INFO;
    static const Level defaultBackTraceLevel = Level::ERROR;

    /** Convert from string to enum */
    static Level getLevelFromString(const std::string&);

    /** Convert from enum to string */
    static const char* getLevelString(const Level);

    static Logger& getLogger(void);

    Logger(const std::string &fileName = "opencog.log");
    Logger(const Logger&) = delete;
    ~Logger();

    Logger& operator=(const Logger&) = delete;

    // Set the level of messages that will be logged.
    void setLevel(Level);
    void setBackTraceLevel(Level);
    void setPrintToStdoutFlag(bool);
    void setTimestampFlag(bool);
    void setFilename(const std::string&);

    // Get the current logging level
    Level getLevel(void) const { return currentLevel; }
    Level getBackTraceLevel(void) const { return backTraceLevel; }
    bool getPrintToStdoutFlag(void) const { return printToStdout; }
    bool getTimestampFlag(void) const { return timestampEnabled; }
    const std::string& getFilename(void) const { return fileName; }

    /**
     * Log a message into log file (passed in constructor)
     *
     * @param level Level of the message
     * @param fmt Format string
     */
    void log(Level level, const char *fmt, ...);
    void logva(Level level, const char *fmt, va_list args);
    void error(const char *fmt, ...);
    void warn(const char *fmt, ...);
    void info(const char *fmt, ...);
    void debug(const char *fmt, ...);
    void fine(const char *fmt, ...);

private:
    std::string fileName;
    bool printToStdout;
    bool timestampEnabled;
    Level currentLevel;
    Level backTraceLevel;
    Level previousLevel;
    std::mutex logMutex;
    FILE *f;

    /**
     * Enable timestamp flag
     */
    void enableTimestamp();

    /**
     * Disable timestamp flag
     */
    void disableTimestamp();
};

} // namespace opencog

#endif // _OPENCOG_LOGGER_H
