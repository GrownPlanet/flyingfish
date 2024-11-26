#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "array_utils.h"
#include "bytecode.h"

void push_chunck(Instructions_t* instr, void* chunck, size_t chunk_size) {
    while (instr->len + chunk_size >= instr->capacity) {
        instr->capacity *= 2;
        instr->chunks = realloc(instr->chunks, instr->capacity);

        if (instr->chunks == NULL) {
            printf("Realloc failed!\n");
            exit(1);
        }
    }

    memcpy(instr->chunks + instr->len, chunck, chunk_size);
    instr->len += chunk_size;
}
