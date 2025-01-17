#include <stdbool.h>
#include <stdio.h>

#include "bytecode.h"
#include "emitter.h"
#include "error.h"

void emit(ByteCode_t* bytecode, char* filename) {
    FILE* fptr;
    fptr = fopen(filename, "w");
    if (fptr == NULL) {
        report_error("Failed to open output file", 0);
    }

    size_t written = fwrite(bytecode->chunks, 1, bytecode->len, fptr);
    if (written != bytecode->len) {
        report_error("Error writing to output file", 0);
    }

    fclose(fptr);
}
