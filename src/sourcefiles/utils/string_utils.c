#include <stddef.h>

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
