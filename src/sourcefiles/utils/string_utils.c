#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "string_utils.h"

bool string_cmp(String_t a, String_t b) {
    if (a.len != b.len) {
        return false;
    }

    for (size_t i = 0; i < a.len; i++) {
        if (a.chars[i] != b.chars[i]) { return false; }
    }

    return true;
}

String_t string_from_chptr(char* orig) {
    return (String_t) {
        .len = strlen(orig),
        .chars = orig,
    };
}

void string_print(String_t str) {
    for (size_t i = 0; i < str.len; i++) {
        printf("%c", str.chars[i]);
    }
}
