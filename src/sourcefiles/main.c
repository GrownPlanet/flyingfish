#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "string_utils.h"
#include "bytecode.h"
#include "compiler.h"
#include "emitter.h"
#include "expression.h"
#include "file_utils.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "interpreter.h"

void print_literal(TokenType_t type, Literal_t* lit);
void print_expression(Expression_t* expression);
void print_help_menu(char* pathname);
int compile_program(char* filename, char* output_filename);
int run_program(char* filename);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        print_help_menu(argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "compile") == 0 || strcmp(argv[1], "c") == 0) {
        return compile_program(argv[2], "out.cff");
    } 
    else if (strcmp(argv[1], "run") == 0 || strcmp(argv[1], "r") == 0) {
        return run_program(argv[2]);
    } 
    else {
        print_help_menu(argv[0]);
        return 1;
    }
}

int compile_program(char* filename, char* output_filename) {
    // reading the file
    String_t file = read_file_to_string(filename, "r");
    if (file.chars == NULL) {
        printf("Error reading file!\n");
        return 1;
    }

    // scanning tokens
    printf("tokens:\n");
    ScanResult_t tokens = scan(file);

    if (tokens.had_error) {
        free_tokens(tokens);
        return 1;
    }

    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        print_token_type(token.type);
        printf(": ");
        print_literal(token.type, token.literal);

        if (i + 1 < tokens.len) {
            printf(", ");
        }
    }
    printf("\n\n");

    // parsing tokens
    printf("ast:\n");
    Expression_t* expr = parse(tokens);

    if (expr == NULL) {
        free_tokens(tokens);
        return 1;
    }

    print_expression(expr);
    printf("\n\nbytecode:\n");

    // compiling the syntax tree
    ByteCode_t bytecode = compile(expr);

    if (bytecode.had_error) {
        free_tokens(tokens);
        free_expression(expr);
        free(bytecode.chunks);
        return 1;
    }

    for (size_t i = 0; i < bytecode.len; i++) {
        printf("%02x ", bytecode.chunks[i]);
    }
    printf("\n");

    // emmitter
    const int res = emit(&bytecode, output_filename);
    if (res == 1) {
        free_tokens(tokens);
        free_expression(expr);
        free(bytecode.chunks);
        return 1;
    }

    // freeing data
    free(file.chars);
    free_tokens(tokens);
    free_expression(expr);
    free(expr);
    free(bytecode.chunks);

    return 0;
}

int run_program(char* filename) {
    // read file
    String_t file = read_file_to_string(filename, "rb");
    if (file.chars == NULL) {
        printf("Error reading file!\n");
        return 1;
    }
    unsigned char* code = (unsigned char*)file.chars;

    // print read file
    for (size_t i = 0; i < file.len; i++) {
        printf("%02x ", code[i]);
    }
    printf("\n");
    
    // interpret the compiled code
    Interpreter_t interpreter = new_interpreter(code, file.len);
    int result = interpret(interpreter);

    // freeing data
    free(file.chars);

    return result;
}

void print_literal(TokenType_t type, Literal_t* lit) {
    switch (type) {
        case TokenType_IntV:
            printf("%ld", lit->i);
            break;
        case TokenType_FloatV:
            printf("%f", lit->f);
            break;
        case TokenType_Identifier:
        case TokenType_StringV:
            printf("\"");
            for (size_t i = 0; i < lit->s->len; i++) {
                printf("%c", lit->s->chars[i]);
            }
            printf("\"");
            break;
        case TokenType_CharV:
            printf("'%c'", lit->c);
            break;
        case TokenType_BoolV:
            printf("%s", lit->b ? "true" : "false");
            break;
        default:
            printf("()");
            break;
    }
}

void print_help_menu(char* pathname) {
    printf("Ussage: `%s {command} {program file | compiled code}`\n", pathname);
    printf("Commands:\n\
compile, c\n\
run, r\n");
}

void print_expression(Expression_t* expr) {
    switch (expr->type) {
        case ExpressionType_Literal: {
            EV_Literal_t* lit = expr->value.literal;
            printf("(");
            print_token_type(lit->type);
            printf(": ");
            print_literal(lit->type, lit->value);
            printf(")");
            break;
        }
        case ExpressionType_Unary: {
            EV_Unary_t* un = expr->value.unary;
            printf("(");
            print_token_type(un->type);
            printf(" ");
            print_token_type(un->operator);
            printf(" ");
            print_expression(&un->operant);
            printf(")");
            break;
        }
        case ExpressionType_Binary: {
            EV_Binary_t* bin = expr->value.binary;
            printf("(");
            print_token_type(bin->type);
            printf(" ");
            print_expression(&bin->left);
            printf(" ");
            print_token_type(bin->operator.type);
            printf(" ");
            print_expression(&bin->right);
            printf(")");
            break;
        }
    }
}
