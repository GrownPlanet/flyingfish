/*
 * Bytecode
 *
 * The bytecode instructions
 *
 * */

#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdlib.h>

typedef enum {
    Instruction_Add,
    Instruction_Div,
    Instruction_Mov,
    Instruction_Mul,
    Instruction_Sub,
} InstructionSet_t;

typedef struct {
    char* chunks;
    size_t len;
    size_t capacity;
} Instructions_t;

void push_chunck(Instructions_t* instructions, void* chunck, size_t chunck_size);

#endif // BYTECODE_H
