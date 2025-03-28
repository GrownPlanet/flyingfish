#include <stdio.h>

#include "numtypes.h"
#include "bytecode.h"
#include "compiler.h"
#include "expression.h"
#include "statement.h"
#include "hashmap.h"

typedef struct {
    ByteCode_t bytecode;
    size_t stack_pointer;
    HashMap_t variables;
} Compiler_t;

int compile_expression(Compiler_t* compiler, Expression_t* expr);
int16_t tokentype_to_flag(TokenType_t tokentype);

// compiles a literal with a new position on the stack, can be used when you want to perform an 
// operation on the literal
int compile_literal_indirect(Compiler_t* compiler, EV_Literal_t* literal) {
    // INSTR MOV
    switch (literal->type) {
        case TokenType_IntV:
        case TokenType_FloatV:
        case TokenType_BoolV:
        case TokenType_CharV:
        case TokenType_Identifier:
            push_chunk(
                &compiler->bytecode,
                (void*)(&(Instruction_t){ Instruction_Mov }),
                sizeof(Instruction_t)
            );
            break;
        case TokenType_StringV:
            push_chunk(
                &compiler->bytecode,
                (void*)(&(Instruction_t){ Instruction_Movs }),
                sizeof(Instruction_t)
            );
            break;
        default:
            printf("Illigal type for literal: ");
            print_token_type(literal->type);
            printf("\n");
            return 1;
            break;
    }
            
    // FLAGS
    int16_t flags = 0;
    if (literal->type == TokenType_Identifier) {
        flags |= ADDRESSING_MODE_INDIRECT;
    } else {
        flags |= ADDRESSING_MODE_DIRECT;
    }
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int16_t)); 
    
    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(Literal_t));

    // ARG2
    switch (literal->type) {
        case TokenType_IntV:
        case TokenType_FloatV:
        case TokenType_BoolV:
        case TokenType_CharV:
            push_chunk(&compiler->bytecode, (void*)literal->value, sizeof(Literal_t));
            break;
        case TokenType_StringV: {
            const String_t* s = literal->value->s;
            push_chunk(&compiler->bytecode, (void*)(&s->len), sizeof(size_t));
            push_chunk(&compiler->bytecode, (void*)s->chars, sizeof(char) * s->len);
            break;
        }
        case TokenType_Identifier: {
            // TODO: fix this, this is inefficient af
            HM_GetResult_t arg = hashmap_get(&compiler->variables, *literal->value->s);
            if (arg.had_error) { return 1; }
            push_chunk(&compiler->bytecode, (void*)(&arg.value), sizeof(Literal_t));
        }
        default: break; // would have already returned an error in previous switch statement
    }

    return 0;
}

typedef struct {
    size_t arg;
    int addressing_mode;
    bool had_error;
} CompileLiteralDirect_t;

// compiles a literal and returns either an address on the stack or the literal itself, can be used
// when you want to add a number to another number already on the stack
CompileLiteralDirect_t compile_literal_direct(Compiler_t* compiler, EV_Literal_t* literal) {
    switch(literal->type) {
        case TokenType_IntV:
        case TokenType_FloatV:
        case TokenType_BoolV:
        case TokenType_CharV:
            return (CompileLiteralDirect_t) {
                .arg = literal->value->u,
                .addressing_mode = ADDRESSING_MODE_DIRECT,
                .had_error = false,
            };
        case TokenType_StringV:
            compile_literal_indirect(compiler, literal);
            // compiler->stack_pointer++;
            return (CompileLiteralDirect_t) {
                .arg = compiler->stack_pointer,
                .addressing_mode = ADDRESSING_MODE_INDIRECT,
                .had_error = false,
            };
        case TokenType_Identifier: {
            HM_GetResult_t arg = hashmap_get(&compiler->variables, *literal->value->s);
            if (arg.had_error) {
                return (CompileLiteralDirect_t) {
                    .arg = 0,
                    .addressing_mode = 0,
                    .had_error = true,
                };
            }
            return (CompileLiteralDirect_t) {
                .arg = arg.value,
                .addressing_mode = ADDRESSING_MODE_DIRECT,
                .had_error = false,
            };
        }
        default:
            return (CompileLiteralDirect_t) {
                .arg = 0,
                .addressing_mode = 0,
                .had_error = true,
            };
    }
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
    CompileLiteralDirect_t res = (CompileLiteralDirect_t) {
        .arg = 0,
        .addressing_mode = ADDRESSING_MODE_INDIRECT,
        .had_error = false,
    };
    if (bin->right.type == ExpressionType_Literal) {
        // just some shitty code, nothing special, it's just that I am aware of that fact here
        if (bin->right.value.literal->type == TokenType_StringV) {
            compiler->stack_pointer++;
            res = compile_literal_direct(compiler, bin->right.value.literal);
            compiler->stack_pointer--;
        } else {
            res = compile_literal_direct(compiler, bin->right.value.literal);
        }
    } else {
        compiler->stack_pointer++;
        compile_expression(compiler, &bin->right);
        compiler->stack_pointer--;
    }
    
    // INSTR
    Instruction_t instr;
    switch (bin->operator.type) {
        case TokenType_Minus: instr = Instruction_Sub; break;
        case TokenType_Plus: instr = Instruction_Add; break;
        case TokenType_Slash: instr = Instruction_Div; break;
        case TokenType_Star: instr = Instruction_Mul; break;
        case TokenType_EqualEqual: instr = Instruction_Eqt; break;
        case TokenType_BangEqual: instr = Instruction_Nqt; break;
        case TokenType_Lesser: instr = Instruction_Let; break;
        case TokenType_Greater: instr = Instruction_Grt; break;
        case TokenType_LesserEqual: instr = Instruction_Lqt; break;
        case TokenType_GreaterEqual: instr = Instruction_Gqt; break;
        default:
            printf("Unexpected token (= ");
            print_token_type(bin->operator.type);
            printf(") in a binary on line %" PRIu "\n", line);
            return 1;
    }
    push_chunk(&compiler->bytecode, (void*)(&instr), sizeof(Instruction_t));

    // FLAGS
    TokenType_t t = bin->in_type;
    int16_t flags = tokentype_to_flag(t);
    if (flags == -1) { return 1; }
    flags |= res.addressing_mode;
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int16_t));

    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));

    // ARG2
    if (bin->right.type == ExpressionType_Literal) {
        push_chunk(&compiler->bytecode, (void*)(&res.arg), sizeof(size_t));
    } else {
        compiler->stack_pointer++;
        push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));
        compiler->stack_pointer--;
    }

    return 0;
}

int compile_expression(Compiler_t* compiler, Expression_t* expr) {
    int res;
    switch (expr->type) {
        case ExpressionType_Literal:
            res = compile_literal_indirect(compiler, expr->value.literal); break;
        case ExpressionType_Unary:
            res = compile_unary(compiler, expr->value.unary, expr->line); break;
        case ExpressionType_Binary:
            res = compile_binary(compiler, expr->value.binary, expr->line); break;
    }
    return res;
}

int compile_print(Compiler_t* compiler, ST_Print_t* print) {
    int res = compile_expression(compiler, print->expr);
    if (res == 1) { return res; }

    // INSTR PRI
    push_chunk(
        &compiler->bytecode,
        (void*)(&(Instruction_t){ Instruction_Pri }),
        sizeof(Instruction_t)
    );

    // FLAGS
    TokenType_t expr_out_t = get_expression_out_type(print->expr);
    int16_t flags;
    if (expr_out_t == TokenType_Identifier) {
        TokenType_t type = 
            hashmap_get(&compiler->variables, *print->expr->value.literal->value->s).type;
        flags = tokentype_to_flag(type);
    } else {
        flags = tokentype_to_flag(expr_out_t);
    }
    if (flags == -1) { return 1; }
    flags |= ADDRESSING_MODE_INDIRECT;
    push_chunk(&compiler->bytecode, (void*)(&flags), sizeof(int16_t));

    // ARG1
    push_chunk(&compiler->bytecode, (void*)(&compiler->stack_pointer), sizeof(size_t));

    return 0;
}

int compile_var(Compiler_t* compiler, ST_Var_t* var) {
    int res = compile_expression(compiler, var->expr);
    res |= hashmap_insert(
        &compiler->variables,
        *var->name,
        compiler->stack_pointer,
        get_expression_out_type(var->expr)
    );

    compiler->stack_pointer++;

    return res;
}

int compile_assignment(Compiler_t* compiler, ST_Assignment_t* assig) {
    int res = compile_expression(compiler, assig->expr);

    Entry_t* entry = hashmap_get_entry(&compiler->variables, *assig->name);
    if (!entry->active) {
        printf("error: cannot find value `");
        string_print(*assig->name);
        printf("` on line %" PRIu " in this scope\n", assig->expr->line);
        return 1;
    }

    if (entry->type != get_expression_out_type(assig->expr)) {
        printf("error: mismatched types on line %" PRIu "\n", assig->expr->line);
        return 1;
    }

    entry->value = compiler->stack_pointer;

    compiler->stack_pointer++;

    return res;
}

int compile_statement(Compiler_t* compiler, Statement_t* stmt) {
    int res = 0;
    switch (stmt->type) {
        case StatementType_Print:
            res = compile_print(compiler, stmt->value.print);
            break;
        case StatementType_Var:
            res = compile_var(compiler, stmt->value.var);
            break;
        case StatementType_Assignment:
            res = compile_assignment(compiler, stmt->value.assignment);
            break;
        default: 
            printf("Unknown statement type: %d\n", stmt->type);
            res = 1;
            break;
    }

    return res;
}

ByteCode_t compile(ParseResult_t parse_result) {
    unsigned char* data = (unsigned char*)malloc(sizeof(unsigned char));
    if (data == NULL) {
        printf("Malloc failed!\n");
        return (ByteCode_t) { .chunks = NULL, .len = 0, .capacity = 0, .had_error = true };
    }

    ByteCode_t bytecode = {
        .chunks = data, 
        .len = 0, 
        .capacity = 1,
        .had_error = false,
    };
    Compiler_t compiler = {
        .bytecode = bytecode, 
        .stack_pointer = 0,
        .variables = new_hashmap(),
    };

    for (size_t i = 0; i < parse_result.len; i++) {
        Statement_t stmt = parse_result.statements[i];
        int res = compile_statement(&compiler, &stmt);
        if (res == 1) { 
            compiler.bytecode.had_error = true; 
            return compiler.bytecode;
        }
    }

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
            printf("Illigal type (");
            print_token_type(tokentype);
            printf(") in tokentype_to_flag\n");
            return -1;
        }
    }
}
