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
    Instruction_AddI,
    Instruction_AddF,
    Instruction_And,
    Instruction_DivI,
    Instruction_DivF,
    Instruction_Mov,
    Instruction_MulI,
    Instruction_MulF,
    Instruction_NegI,
    Instruction_NegF,
    Instruction_Not,
    Instruction_Or,
    Instruction_SubI,
    Instruction_SubF,
    Instruction_Xor,
} Instruction_t;

typedef enum {
    AddressingMode_Direct,
    AddressingMode_Indirect,
} AddressingMode_t;

typedef struct {
    unsigned char* chunks;
    size_t len;
    size_t capacity;
} ByteCode_t;

void push_chunk(ByteCode_t* bytecode, void* chunck, size_t chunck_size);

#endif // BYTECODE_H
