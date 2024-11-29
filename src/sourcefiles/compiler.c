#include <stdio.h>

#include "bytecode.h"
#include "compiler.h"
#include "expression.h"
#include "error.h"

void compile_expression(ByteCode_t* bytecode, Expression_t* expr);

void compile_literal(ByteCode_t* bytecode, EV_Literal_t* literal) {
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
    push_chunk(bytecode, (void*)literal->value, type_size);
}

void compile_unary(ByteCode_t* bytecode, EV_Unary_t* unary, size_t line) {
    InstructionSet_t instr;
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
    push_chunk(bytecode, (void*)(&instr), sizeof(InstructionSet_t));
    compile_expression(bytecode, &unary->operant);
}

void compile_expression(ByteCode_t* bytecode, Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Literal:
            compile_literal(bytecode, expr->value.literal);
            break;
        case ExpressionType_Unary:
            compile_unary(bytecode, expr->value.unary, expr->line);
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

    compile_expression(&bytecode, expr);

    return bytecode;
}
