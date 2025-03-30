#include <stdbool.h>
#include <stdio.h>

#include "bytecode.h"
#include "emitter.h"

int emit(ByteCode_t* bytecode, char* filename) {
    FILE* fptr;
    fptr = fopen(filename, "wb");
    if (fptr == NULL) {
        printf("error: failed to open output file\n");
        return 1;
    }

    size_t written = fwrite(bytecode->chunks, 1, bytecode->len, fptr);
    if (written != bytecode->len) {
        printf("error: failed to write to output file\n");
        return 1;
    }

    fclose(fptr);
    return 0;
}
