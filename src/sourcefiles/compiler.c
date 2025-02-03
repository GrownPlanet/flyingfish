#include <stdio.h>

#include "bytecode.h"
#include "compiler.h"
#include "expression.h"
#include "error.h"

typedef struct {
    ByteCode_t bytecode;
    size_t stack_pointer;
} Compiler_t;

int compile_expression(Compiler_t* compiler, Expression_t* expr);
TokenType_t get_type(Expression_t* expr);
int tokentype_to_flag(TokenType_t tokentype);

void compile_literal(Compiler_t* compiler, EV_Literal_t* literal) {
    // INSTR MOV
    push_chunk(
        &compiler->bytecode,
        (void*)(&(Instruction_t){ Instruction_Mov }),
        sizeof(Instruction_t)
    );
    // FLAGS
    const int flags = ADDRESSING_MODE_DIRECT;
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int));
    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(Literal_t));
    // ARG2
    push_chunk(&compiler->bytecode, (void*)literal->value, sizeof(Literal_t));
    printf("MOV %x, %llx, %llx\n", flags, compiler->stack_pointer, literal->value->i);
}

int compile_unary(Compiler_t* compiler, EV_Unary_t* unary, size_t line) {
    // compile the operant
    compile_expression(compiler, &unary->operant);

    // INSTR
    Instruction_t instr;
    switch (unary->operator) {
        case TokenType_Minus: 
            instr = Instruction_Neg;
            break;
        case TokenType_Bang: 
            instr = Instruction_Not; 
            break;
        default:
            report_error("unexpected token in a unary", line);
            break;
    }
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));

    // FLAGS
    const int flags = tokentype_to_flag(get_type(&unary->operant));
    if (flags == -1) { return flags; }
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int));

    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));

    return 0;
}

int compile_binary(Compiler_t* compiler, EV_Binary_t* bin, size_t line) {
    // compile the operants
    compile_expression(compiler, &bin->left);
    compiler->stack_pointer++;
    compile_expression(compiler, &bin->right);
    compiler->stack_pointer--;
    
    // INSTR
    Instruction_t instr;
    switch (bin->operator.type) {
        case TokenType_Minus:
            instr = Instruction_Sub;
            break;
        case TokenType_Plus:
            instr = Instruction_Add;
            break;
        case TokenType_Slash:
            instr = Instruction_Div;
            break;
        case TokenType_Star:
            instr = Instruction_Mul;
            break;
        default:
            report_error("unexpected token in a binary", line);
            break;
    }
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));

    // FLAGS
    TokenType_t t1 = get_type(&bin->left);
    if (t1 == -1) { return t1; }
    TokenType_t t2 = get_type(&bin->right);
    if (t2 == -1) { return t2; }

    if (t1 != t2) {
        printf("Type Error: binary type 1 (= %d) != type 2 (= %d)\n", t1, t2);
        return -1;
    }

    const int flags = tokentype_to_flag(t1) | ADDRESSING_MODE_INDIRECT;
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int));

    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));

    // ARG2
    compiler->stack_pointer++;
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));
    compiler->stack_pointer--;

    return 0;
}

int compile_expression(Compiler_t* compiler, Expression_t* expr) {
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
            return 1;
    }
    return 0;
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

TokenType_t get_type(Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Binary: {
            TokenType_t t1 = get_type(&expr->value.binary->left);
            TokenType_t t2 = get_type(&expr->value.binary->right);
            if (t1 == t2) {
                return t1;
            } else {
                printf("Type error: type 1 (= %d) != type 2 (= %d)\n", t1, t2);
                return -1;
            }
        }
        case ExpressionType_Unary: {
            return get_type(&expr->value.unary->operant);
        }
        case ExpressionType_Literal: {
            return expr->value.literal->type;
        }
        default: {
            return -1;
        }
    }
}

int tokentype_to_flag(TokenType_t tokentype) {
    switch (tokentype) {
        case TokenType_IntV: { return TYPE_INT; }
        case TokenType_FloatV: { return TYPE_FLOAT; }
        case TokenType_CharV: { return TYPE_CHAR; }
        case TokenType_StringV: { return TYPE_STRING; }
        case TokenType_BoolV: { return TYPE_BOOL; }
        default: {
            printf("Illigal type (%d) in tokentype_to_flag\n", tokentype);
            return -1;
        }
    }
}
