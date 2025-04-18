/*
 * Array utils
 *
 * Simple operations for files.
 *
 */

#ifndef ARRAY_UTILS_H
#define ARRAY_UTILS_H

#include <stddef.h>

void push(void** arr, size_t* len, size_t* capacity, size_t elem_size, const void* elem);

#endif // ARRAY_UTILS_H
