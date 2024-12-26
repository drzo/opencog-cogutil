#ifndef _OPENCOG_COMPAT_H
#define _OPENCOG_COMPAT_H

// This header provides compatibility definitions for various C++ operators
// that might not be available in all compilers or platforms

#ifdef _MSC_VER
    // MSVC doesn't define the and/or/not operators
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

#endif // _OPENCOG_COMPAT_H 