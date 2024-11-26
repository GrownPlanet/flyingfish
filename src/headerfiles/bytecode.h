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
    InstructionSet_t* instructions;
    size_t len;
} Instructions_t;

void push_instruction(Instructions_t* instr);

#endif // BYTECODE_H
