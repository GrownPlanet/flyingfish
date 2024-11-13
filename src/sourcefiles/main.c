#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "file_utils.h"
#include "scanner.h"
#include "token.h"
#include "error.h"
#include "parser.h"

void print_literal(TokenType_t type, Literal_t* lit);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Ussage: %s {program file}.\n", argv[0]);
        return 1;
    }

    init_error();

    // reading the file
    char* file = read_file_to_string(argv[1]);
    if (file == NULL) {
        printf("Error reading file!\n");
        return 1;
    }

    // scanning tokens
    ScanResult_t tokens = scan(file, strlen(file));

    if (had_error()) {
        print_errors();
        free_error();
        return 1;
    }

    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        printf("%d: ", token.type);
        print_literal(token.type, token.literal);

        if (i + 1 < tokens.len) {
            printf(", ");
        }
    }
    printf("\n");

    // parsing tokens
    Expression_t* expr = parse(tokens.tokens, tokens.len);

    if (had_error()) {
        print_errors();
        free_error();
        return 1;
    }

    switch (expr->type) {
        case ExpressionType_Literal:
            EV_Literal_t* lit = expr->value.literal;
            printf("%d: ", lit->type);
            print_literal(lit->type, lit->value);
            printf("\n");
            break;
        default:
            return 1;
    }

    // freeing data
    free(file);
    free_error();
    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        if (token.type == TokenType_StringV || token.type == TokenType_Identifier) {
            free(token.literal->str);
        }
        free(token.literal);
    }
    free(tokens.tokens);

    return 0;
}

void print_literal(TokenType_t type, Literal_t* lit) {
    switch (type) {
        case TokenType_IntV:
            printf("%ld", lit->ln);
            break;
        case TokenType_FloatV:
            printf("%f", lit->db);
            break;
        case TokenType_Identifier:
        case TokenType_StringV:
            printf("\"%s\"", lit->str);
            break;
        case TokenType_CharV:
            printf("'%c'", lit->ch);
            break;
        case TokenType_BoolV:
            printf("%s", lit->b ? "true" : "false");
            break;
        default:
            printf("()");
            break;
    }
}
