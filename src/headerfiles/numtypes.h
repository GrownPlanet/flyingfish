/*
 * Numtypes
 *
 * Automatic types for numbers depending on if the program is 64 bit or 32 bit
 *
 * */

#ifndef NUMTYPES_H
#define NUMTYPES_H

#include <inttypes.h>

#if defined(__x86_64__) || defined(_M_X64) // x64
    typedef int64_t int_t;
    typedef double float_t;

    // for printing the type
    #define PRIx PRIx64
    #define PRId PRId64
    #define PRIu PRIu64

    #define ARCH "x64"
#elif defined(__i386) || defined(_M_IX86) // x32
    typedef int32_t int_t;
    typedef float float_t;

    // for printing the type
    #define PRIx PRIx32
    #define PRId PRId32
    #define PRIu PRIu32

    #define ARCH "x32"
#else
    #define ARCH "unknown"
#endif

#endif // NUMTYPES_H
