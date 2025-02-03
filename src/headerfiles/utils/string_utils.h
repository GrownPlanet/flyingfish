/*
 * String
 *
 * A more modern alternative for char*
 *
 * */

#ifndef STRING_H
#define STRING_H

#include <stdbool.h>

typedef struct {
    char* chars;
    size_t len;
} String_t;

bool string_cmp(String_t a, String_t b);

#endif // STRING_H
