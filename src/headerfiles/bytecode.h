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
    Instruction_And,
    Instruction_Div,
    Instruction_Mov,
    Instruction_Mul,
    Instruction_Neg,
    Instruction_Not,
    Instruction_Or,
    Instruction_Sub,
    Instruction_Xor,
} Instruction_t;


#define ADDRESSING_MODE_PART     1 // 0b1
#define ADDRESSING_MODE_DIRECT   0 // 0b0
#define ADDRESSING_MODE_INDIRECT 1 // 0b1

#define TYPE_PART   7 << 1 // 0b111
#define TYPE_INT    0 << 1 // 0b000
#define TYPE_FLOAT  1 << 1 // 0b001
#define TYPE_CHAR   2 << 1 // 0b010
#define TYPE_STRING 3 << 1 // 0b011
#define TYPE_BOOL   4 << 1 // 0b100

typedef struct {
    unsigned char* chunks;
    size_t len;
    size_t capacity;
} ByteCode_t;

void push_chunk(ByteCode_t* bytecode, void* chunck, size_t chunck_size);

#endif // BYTECODE_H
