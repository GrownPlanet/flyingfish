#include <stdio.h>

#include "bytecode.h"
#include "compiler.h"
#include "expression.h"
#include "error.h"

typedef struct {
    ByteCode_t bytecode;
    size_t stack_pointer;
} Compiler_t;

// TODO: add support for floats!
// => float and int bits in a union (Literal_t)

void compile_expression(Compiler_t* compiler, Expression_t* expr);

void compile_literal(Compiler_t* compiler, EV_Literal_t* literal) {
    // INSTR MOV
    push_chunk(
        &compiler->bytecode,
        (void*)(&(Instruction_t){Instruction_Mov}),
        sizeof(Instruction_t)
    );
    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));
    // ADR2
    push_chunk(
        &compiler->bytecode,
        (void*)(&(AddressingMode_t){AddressingMode_Direct}),
        sizeof(AddressingMode_t)
    );
    // ARG2
    push_chunk(&compiler->bytecode, (void*)literal->value, sizeof(literal->value));
}

void compile_unary(Compiler_t* compiler, EV_Unary_t* unary, size_t line) {
    // compile the operant
    compile_expression(compiler, &unary->operant);

    // INSTR
    Instruction_t instr;
    switch (unary->operator) {
        case TokenType_Minus: 
            instr = Instruction_NegI; 
            break;
        case TokenType_Bang: 
            instr = Instruction_Not; 
            break;
        default:
            report_error("unexpected token in a unary", line);
            break;
    }
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));

    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));
}

void compile_binary(Compiler_t* compiler, EV_Binary_t* bin, size_t line) {
    // compile the operants
    compile_expression(compiler, &bin->left);
    compiler->stack_pointer++;
    compile_expression(compiler, &bin->right);
    compiler->stack_pointer--;
    
    // INSTR
    Instruction_t instr;
    switch (bin->operator.type) {
        case TokenType_Minus:
            instr = Instruction_SubI;
            break;
        case TokenType_Plus:
            instr = Instruction_AddI;
            break;
        case TokenType_Slash:
            instr = Instruction_DivI;
            break;
        case TokenType_Star:
            instr = Instruction_MulI;
            break;
        default:
            report_error("unexpected token in a binary", line);
            break;
    }
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));
    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));
    // ADR2
    push_chunk(
        &compiler->bytecode,
        (void*)(&(AddressingMode_t){AddressingMode_Indirect}),
        sizeof(AddressingMode_t)
    );
    // ARG2
    compiler->stack_pointer++;
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));
    compiler->stack_pointer--;
}

void compile_expression(Compiler_t* compiler, Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Literal:
            compile_literal(compiler, expr->value.literal);
            break;
        case ExpressionType_Unary:
            compile_unary(compiler, expr->value.unary, expr->line);
            break;
        case ExpressionType_Binary:
            compile_binary(compiler, expr->value.binary, expr->line);
            break;
        default:
            printf("TODO 01: handle more expression types: compiler.c\n");
            exit(1);
            break;
    }
}

ByteCode_t compile(Expression_t* expr) {
    unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char));

    ByteCode_t bytecode = {
        .chunks = data, 
        .len = 0, 
        .capacity = 1,
    };
    Compiler_t compiler = {
        .bytecode = bytecode, 
        .stack_pointer = 0,
    };

    compile_expression(&compiler, expr);

    return compiler.bytecode;
}
