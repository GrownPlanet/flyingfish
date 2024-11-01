#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_utils.h"

void push(void** arr, size_t* len, size_t* capacity, size_t elem_size, const void* elem) {
    if (*len >= *capacity) {
        *capacity *= 2;
        *arr = realloc(*arr, (*capacity) * elem_size);

        if (*arr == NULL) {
            printf("Realloc failed!\n");
            exit(1);
        }
    }

    memcpy((char*)*arr + (*len * elem_size), elem, elem_size);
    (*len)++;
}
