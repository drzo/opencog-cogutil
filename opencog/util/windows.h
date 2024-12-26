#ifndef _OPENCOG_WINDOWS_H
#define _OPENCOG_WINDOWS_H

#ifdef _MSC_VER

// Define Windows-specific macros and includes
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <io.h>
#include <direct.h>

// POSIX compatibility
#define F_OK 0
#define PATH_MAX MAX_PATH
#define getcwd _getcwd
#define chdir _chdir
#define mkdir(a, b) _mkdir(a)
#define access _access
#define snprintf _snprintf
#define vsnprintf _vsnprintf
#define strcasecmp _stricmp
#define strncasecmp _strnicmp

// C++ alternative operators
#ifndef __cplusplus
#define and &&
#define or ||
#define not !
#define xor ^
#define and_eq &=
#define or_eq |=
#define not_eq !=
#define xor_eq ^=
#define bitand &
#define bitor |
#define compl ~
#endif

// Disable MSVC warnings
#pragma warning(disable: 4290)  // throw specification ignored
#pragma warning(disable: 4996)  // posix function deprecation warnings
#pragma warning(disable: 4800)  // forcing value to bool

#endif // _MSC_VER

#endif // _OPENCOG_WINDOWS_H 