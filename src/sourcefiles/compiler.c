#include <stdio.h>

#include "numtypes.h"
#include "bytecode.h"
#include "compiler.h"
#include "expression.h"
#include "statement.h"

typedef struct {
    ByteCode_t bytecode;
    size_t stack_pointer;
} Compiler_t;

int compile_expression(Compiler_t* compiler, Expression_t* expr);
int16_t tokentype_to_flag(TokenType_t tokentype);

void compile_literal(Compiler_t* compiler, EV_Literal_t* literal) {
    // INSTR MOV
    push_chunk(
        &compiler->bytecode,
        (void*)(&(Instruction_t){ Instruction_Mov }),
        sizeof(Instruction_t)
    );
    // FLAGS
    const int16_t flags = ADDRESSING_MODE_DIRECT;
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int16_t)); 
    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(Literal_t));
    // ARG2
    push_chunk(&compiler->bytecode, (void*)literal->value, sizeof(Literal_t));
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
            printf("Unexpected token (= ");
            print_token_type(unary->operator);
            printf(") in a unary on line %" PRIu "\n", line);
            return 1;
    }
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));

    // FLAGS
    const int16_t flags = tokentype_to_flag(unary->type);
    if (flags == -1) { return 1; }
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int16_t));

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
        case TokenType_EqualEqual:
            instr = Instruction_Eqt;
            break;
        case TokenType_BangEqual:
            instr = Instruction_Nqt;
            break;
        case TokenType_Lesser:
            instr = Instruction_Let;
            break;
        case TokenType_Greater:
            instr = Instruction_Grt;
            break;
        case TokenType_LesserEqual:
            instr = Instruction_Lqt;
            break;
        case TokenType_GreaterEqual:
            instr = Instruction_Gqt;
            break;
        default:
            printf("Unexpected token (= ");
            print_token_type(bin->operator.type);
            printf(") in a binary on line %" PRIu "\n", line);
            return 1;
    }
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));

    // FLAGS
    TokenType_t t = bin->in_type;
    const int16_t flags = tokentype_to_flag(t) | ADDRESSING_MODE_INDIRECT;
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int16_t));

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
        case ExpressionType_Literal: {
            compile_literal(compiler, expr->value.literal);
            break;
        }
        case ExpressionType_Unary: {
            int res = compile_unary(compiler, expr->value.unary, expr->line);
            if (res == 1) { return res; }
            break;
        }
        case ExpressionType_Binary: {
            int res = compile_binary(compiler, expr->value.binary, expr->line);
            if (res == 1) { return res; }
            break;
        }
    }
    return 0;
}

int compile_print(Compiler_t* compiler, ST_Print_t* print) {
    compile_expression(compiler, print->expr);

    // INSTR PRI
    push_chunk(
        &compiler->bytecode,
        (void*)(&(Instruction_t){ Instruction_Pri }),
        sizeof(Instruction_t)
    );

    // FLAGS
    int16_t flags = tokentype_to_flag(get_expression_out_type(print->expr));
    if (flags == -1) { return 1; }
    flags |= ADDRESSING_MODE_INDIRECT;
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int16_t));

    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));

    return 0;
}

int compile_statement(Compiler_t* compiler, Statement_t* stmt) {
    switch (stmt->type) {
        case StatementType_Print: {
            int res = compile_print(compiler, stmt->value.print);
            if (res == 1) { return res; }
            break;
        }
        default: 
            printf("Unknown statement type: %d\n", stmt->type);
            return 1;
    }
    return 0;
}

ByteCode_t compile(Statement_t* stmt) {
    unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char));

    ByteCode_t bytecode = {
        .chunks = data, 
        .len = 0, 
        .capacity = 1,
        .had_error = false,
    };
    Compiler_t compiler = {
        .bytecode = bytecode, 
        .stack_pointer = 0,
    };

    // int res = compile_expression(&compiler, expr);
    int res = compile_statement(&compiler, stmt);
    if (res == 1) { compiler.bytecode.had_error = true; }

    return compiler.bytecode;
}

int16_t tokentype_to_flag(TokenType_t tokentype) {
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
