#include <stdbool.h>
#include <stdio.h>

#include "array_utils.h"
#include "error.h"

Error_t* ERRORS;
size_t ERRORS_LEN = 0;
size_t ERRORS_CAPACITY = 0;

void init_error() {
    ERRORS = (Error_t*)malloc(sizeof(Error_t));
    if (ERRORS == NULL) {
        printf("malloc failed!\n");
        exit(1);
    }
    ERRORS_CAPACITY = 1;
}

void report_error(char* message, size_t line) {
    Error_t error = {
        message, 
        line
    };
    push((void**)&ERRORS, &ERRORS_LEN, &ERRORS_CAPACITY, sizeof(Error_t), &error);
}

bool had_error() {
    return ERRORS_LEN != 0;
}

void print_errors() {
    Error_t error;
    for (size_t i = 0; i < ERRORS_LEN; i++) {
        error = ERRORS[i];
        printf("%s on line %ld\n", error.message, error.line);
    }
}

void free_error() {
    free(ERRORS); 
}
