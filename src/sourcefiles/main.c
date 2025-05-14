/*
 * TODO: 
 *  - add optional types to var's
 *  - improve recalling a variable (see todo in compiler.c)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "numtypes.h"
#include "utils/string.h"
#include "bytecode.h"
#include "compiler.h"
#include "emitter.h"
#include "ast/expression.h"
#include "ast/statement.h"
#include "utils/file.h"
#include "scanner.h"
#include "token.h"
#include "parser.h"
#include "interpreter.h"
#include "hashmap.h"

void print_literal(TokenType_t type, Literal_t* lit);
void print_statement(Statement_t* stmt);
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

    if (strcmp(argv[1], "run") == 0 || strcmp(argv[1], "r") == 0) {
        return run_program(argv[2]);
    } 

    print_help_menu(argv[0]);
    return 1;
}

int compile_program(char* filename, char* output_filename) {
    printf("Compiling for %s\n", ARCH);

    // reading the file
    String_t file = read_file_to_string(filename, "r");
    if (file.chars == NULL) {
        printf("error while reading file\n");
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
    ParseResult_t parse_result = parse(tokens);

    if (parse_result.had_error) {
        free_tokens(tokens);
        return 1;
    }

    for (size_t i = 0; i < parse_result.len; i++) {
        Statement_t stmt = parse_result.statements[i];
        print_statement(&stmt);
        printf("\n");
    }

    printf("\nbytecode:\n");

    // compiling the syntax tree
    ByteCode_t bytecode = compile(parse_result);

    if (bytecode.had_error) {
        free_tokens(tokens);
        for (size_t i = 0; i < parse_result.len; i++) {
            free_statement(&parse_result.statements[i]);
        }
        free(bytecode.chunks);
        return 1;
    }

    for (size_t i = 0; i < bytecode.len; i++) {
        printf("%02x ", bytecode.chunks[i]);
    }
    printf("\n");

    // emmitter
    int res = emit(&bytecode, output_filename);

    // freeing data
    free(file.chars);
    free_tokens(tokens);
    for (size_t i = 0; i < parse_result.len; i++) {
        free_statement(&parse_result.statements[i]);
    }
    free(bytecode.chunks);

    return res;
}

int run_program(char* filename) {
    printf("Running program in %s\n", ARCH);

    // read file
    printf("bytecode:\n");
    String_t file = read_file_to_string(filename, "rb");
    if (file.chars == NULL) {
        printf("error while reading file\n");
        return 1;
    }
    unsigned char* code = (unsigned char*)file.chars;

    // print read file
    for (size_t i = 0; i < file.len; i++) {
        printf("%02x ", code[i]);
    }
    printf("\n\n");
    
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
            printf("%" PRId "", lit->i);
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
            print_token_type(bin->in_type);
            printf(" -> ");
            print_token_type(bin->out_type);
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

void print_statement(Statement_t* stmt) {
    switch (stmt->type) {
        case StatementType_Print: {
            ST_Print_t* pr = stmt->value.print;
            printf("(Print ");
            print_expression(pr->expr);
            printf(")");
            break;
        }
        case StatementType_Var: {
            ST_Var_t* var = stmt->value.var;
            printf("(Var ");
            string_print(*var->name);
            printf(" ");
            print_token_type(var->type);
            printf(" ");
            print_expression(var->expr);
            printf(")");
            break;
        }
        case StatementType_Assignment: {
            ST_Assignment_t* assig = stmt->value.assignment;
            printf("(Assignment ");
            string_print(*assig->name);
            printf(" ");
            print_expression(assig->expr);
            printf(")");
            break;
        }
        case StatementType_Block: {
            ST_Block_t* block = stmt->value.block;
            printf("(Block ");
            for (size_t i = 0; i < block->len - 1; i++) {
                print_statement(&block->stmts[i]);
                printf(" ");
            }
            print_statement(&block->stmts[block->len - 1]);
            printf(")");
            break;
        }
        case StatementType_If: {
            ST_If_t* if_s = stmt->value.if_s;
            printf("(If ");
            print_expression(if_s->condition);
            printf(" ");
            print_statement(if_s->if_body);
            if (if_s->else_body != NULL) {
                printf(" Else ");
                print_statement(if_s->else_body);
            }
            break;
        }
        case StatementType_While: {
            ST_While_t* while_s = stmt->value.while_s;
            printf("(While ");
            print_expression(while_s->condition);
            printf(" ");
            print_statement(while_s->body);
            printf(")");
            break;
        }
        case StatementType_For: {
            ST_For_t* for_s = stmt->value.for_s;
            printf("(For ");
            print_statement(for_s->init);
            printf(" ");
            print_expression(for_s->condition);
            printf(" ");
            print_statement(for_s->incr);
            printf(" ");
            print_statement(for_s->body);
            printf(")");
            break;
        }
        case StatementType_Function: {
            ST_Function_t* func = stmt->value.function;
            printf("(Func ");
            string_print(*func->name);
            printf(" (");
            for (size_t i = 0; i < func->input.len; i++) {
                printf(" (");
                print_token_type(func->input.types[i]);
                printf(" ");
                string_print(func->input.names[i]);
                printf(")");
            }
            printf(") ");
            print_statement(func->body);
            printf(")");
            break;
        }
    }
}
