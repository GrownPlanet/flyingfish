#include <stdio.h>

#include "bytecode.h"
#include "compiler.h"
#include "expression.h"
#include "error.h"

typedef struct {
    ByteCode_t bytecode;
    size_t stack_pointer;
} Compiler_t;

void compile_expression(Compiler_t* compiler, Expression_t* expr);

void compile_literal(Compiler_t* compiler, EV_Literal_t* literal) {
    size_t type_size = 0;
    switch (literal->type) {
        case TokenType_IntV:
            type_size = sizeof(long);
            break;
        case TokenType_FloatV:
            type_size = sizeof(double);
            break;
        case TokenType_CharV:
            type_size = sizeof(char);
            break;
        case TokenType_BoolV:
            type_size = sizeof(bool);
            break;
        default:
            printf("TODO 02: Handle strings: compiler.c\n");
            exit(1);
            break;
    }
    Instruction_t instr = Instruction_Mov;
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));
    // TODO: push stack index to
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));
    push_chunk(&compiler->bytecode, (void*)literal->value, type_size);
}

void compile_unary(Compiler_t* compiler, EV_Unary_t* unary, size_t line) {
    Instruction_t instr;
    switch (unary->operator) {
        case TokenType_Minus:
            instr = Instruction_Neg;
            break;
        case TokenType_Bang:
            // TODO: change this to be an xor with 1 when more logic is implemented
            instr = Instruction_Not;
            break;
        default:
            report_error("unexpected token in a unary", line);
            break;
    }
    compile_expression(compiler, &unary->operant);
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));
    // TODO: push stack index to not
}

void compile_expression(Compiler_t* compiler, Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Literal:
            compile_literal(compiler, expr->value.literal);
            break;
        case ExpressionType_Unary:
            compile_unary(compiler, expr->value.unary, expr->line);
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
        data, 0, 1,
    };
    Compiler_t compiler = {
        bytecode, 0
    };

    compile_expression(&compiler, expr);

    return compiler.bytecode;
}
