#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_utils.h"
#include "bytecode.h"

void push_chunk(ByteCode_t* bytecode, void* chunk, size_t chunk_size) {
    while (bytecode->len + chunk_size >= bytecode->capacity) {
        bytecode->capacity *= 2;
        bytecode->chunks = realloc(bytecode->chunks, bytecode->capacity);

        if (bytecode->chunks == NULL) {
            printf("Realloc failed!\n");
            exit(1);
        }
    }

    memcpy(bytecode->chunks + bytecode->len, chunk, chunk_size);

    bytecode->len += chunk_size;
}
