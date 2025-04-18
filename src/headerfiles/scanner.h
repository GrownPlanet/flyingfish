/*
 * Scanner
 *
 * Extracts tokens from the source code of the program.
 *
 * */

#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"
#include "utils/string.h"

typedef struct {
    Token_t* tokens;
    size_t len;
    bool had_error;
} ScanResult_t;

ScanResult_t scan(String_t input);
void free_tokens(ScanResult_t tokens);

#endif // SCANNER_H
