/*
 * src/util/misc.h
*
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008 by OpenCog Foundation
 * All Rights Reserved
 *
 * Written by Thiago Maia <thiago@vettatech.com>
 *            Andre Senna <senna@vettalabs.com>
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

#include "misc.h"
#include <cstdlib>
#include <string>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <dbghelp.h>
#pragma comment(lib, "dbghelp.lib")
#else
#include <cxxabi.h>
#include <unistd.h>
#endif

using namespace opencog;

// MIT HAKMEM Count
unsigned int opencog::bitcount(unsigned long n)
{
    /* works for 32-bit numbers only    */
    /* fix last line for 64-bit numbers */
    unsigned long tmp;

    tmp = n - ((n >> 1) & 033333333333)
          - ((n >> 2) & 011111111111);
    return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}

std::string opencog::demangle(const std::string& mangled)
{
#ifdef _MSC_VER
    char undecorated_name[1024];
    if (UnDecorateSymbolName(mangled.c_str(), undecorated_name,
                            sizeof(undecorated_name), UNDNAME_COMPLETE))
        return std::string(undecorated_name);
    return mangled;
#else
    return mangled;
#endif
}

