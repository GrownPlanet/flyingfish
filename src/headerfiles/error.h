#pragma once

#include <stdlib.h>

typedef struct {
    char* message;
    size_t line;
} Error_t;

extern Error_t* ERRORS;
extern size_t ERRORS_LEN;
extern size_t ERRORS_CAPACITY;

void init_error();
void report_error(char* message, size_t line);
bool had_error();
void print_errors();
void free_error();
