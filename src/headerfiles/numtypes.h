/*
 * Numtypes
 *
 * Automatic types for numbers depending on if the program is 64 bit or 32 bit
 *
 * */

#ifndef NUMTYPES_H
#define NUMTYPES_H

#include <inttypes.h>

#if defined(__LP64__) || defined(_LP64)
    typedef int64_t int_t;
    typedef double float_t;

    // for printing the type
    #define PRIx PRIx64
    #define PRId PRId64
    #define PRIu PRIu64

    #define ARCH "x64"
#else
    typedef int32_t int_t;
    typedef float float_t;

    // for printing the type
    #define PRIx PRIx32
    #define PRId PRId32
    #define PRIu PRIu32

    #define ARCH "x32"
#endif

#endif // NUMTYPES_H
