/*
 * Scanner
 *
 * Extracts tokens from the source code of the program.
 *
 * */

#pragma once

#include "token.h"

typedef struct {
    Token_t* tokens;
    size_t len;
} ScanResult_t;

ScanResult_t scan(char* input, size_t input_len);
