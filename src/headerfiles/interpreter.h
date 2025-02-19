/*
 * Interpreter
 *
 * Interpret the compiled source code
 *
 * */

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "bytecode.h"
#include "token.h"

typedef struct {
    Literal_t* data;
    size_t capacity;
} Stack_t;

typedef struct {
    unsigned char* code;
    size_t len;
    size_t instr_ptr;
    Stack_t stack;
} Interpreter_t;

Interpreter_t new_interpreter(unsigned char* code, size_t len);

int interpret(Interpreter_t interpreter);

#endif // INTERPRETER_H
