#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/array.h"
#include "bytecode.h"

void push_chunk(ByteCode_t* bytecode, void* chunk, size_t chunk_size) {
    while (bytecode->len + chunk_size >= bytecode->capacity) {
        bytecode->capacity *= 2;
        bytecode->chunks = realloc(bytecode->chunks, bytecode->capacity);

        if (bytecode->chunks == NULL) {
            printf("realloc failed!\n");
            exit(1);
        }
    }

    memcpy(bytecode->chunks + bytecode->len, chunk, chunk_size);

    bytecode->len += chunk_size;
}

void print_var_type(int type) {
    switch (type) {
        case TYPE_INT: printf("int"); break;
        case TYPE_FLOAT: printf("float"); break;
        case TYPE_CHAR: printf("char"); break;
        case TYPE_STRING: printf("string"); break;
        case TYPE_BOOL: printf("bool"); break;
    }
}
