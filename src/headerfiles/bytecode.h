/*
 * Bytecode
 *
 * The bytecode instructions
 *
 * */

#ifndef BYTECODE_H
#define BYTECODE_H

#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    // [instr] [flags] [num1] [num2]
    Instruction_Add,
    Instruction_And,
    Instruction_Div,
    Instruction_Mul,
    Instruction_Or,
    Instruction_Sub,
    Instruction_Xor,
    Instruction_Eqt,
    Instruction_Nqt,
    Instruction_Let,
    Instruction_Grt,
    Instruction_Lqt,
    Instruction_Gqt,
    // [instr] [flags] [n]
    Instruction_Not,
    Instruction_Neg,
    // MOV [flags] [loc] [num]
    Instruction_Mov,
    // MOVS [flags] [loc] [len] [char](*len)
    Instruction_Movs,
    // PRI [flags] [num]
    Instruction_Pri,
    // JMP [location]
    Instruction_Jmp,
    // IF [flags] [cond]
    // JMP [else location]
    // [then case]
    // JMP [after if location]?
    // [else case]?
    Instruction_If,
} Instruction_t;

// flags
#define ADDRESSING_MODE_PART     1 // 0b1
#define ADDRESSING_MODE_DIRECT   0 // 0b0
#define ADDRESSING_MODE_INDIRECT 1 // 0b1

#define TYPE_PART   7 << 1 // 0b1110

#define TYPE_INT    0 << 1 // 0b000
#define TYPE_FLOAT  1 << 1 // 0b001
#define TYPE_CHAR   2 << 1 // 0b010
#define TYPE_STRING 3 << 1 // 0b011
#define TYPE_BOOL   4 << 1 // 0b100

typedef struct {
    unsigned char* chunks;
    size_t len;
    size_t capacity;
    bool had_error;
} ByteCode_t;

void push_chunk(ByteCode_t* bytecode, void* chunck, size_t chunck_size);
void print_var_type(int type);

#endif // BYTECODE_H
