/*
 * String
 *
 * A more modern alternative for char*
 *
 * */

#ifndef STRING_H
#define STRING_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

typedef struct {
    char* chars;
    size_t len;
} String_t;

bool string_cmp(String_t a, String_t b);
String_t string_from_chptr(char* orig);
void string_print(String_t str);

#endif // STRING_H
