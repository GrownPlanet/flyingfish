#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expression.h"
#include "file_utils.h"
#include "scanner.h"
#include "token.h"
#include "error.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Ussage: %s {program file}.\n", argv[0]);
        return 1;
    }

    init_error();

    char* file = read_file_to_string(argv[1]);
    if (file == NULL) {
        printf("Error reading file!\n");
        return 1;
    }

    ScanResult_t tokens = scan(file, strlen(file));

    if (had_error()) {
        print_errors();
        free_error();
        return 1;
    }

    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        printf("%d: ", token.type);
        switch (token.type) {
            case TokenType_IntV:
                printf("%ld", token.literal->ln);
                break;
            case TokenType_FloatV:
                printf("%f", token.literal->db);
                break;
            case TokenType_Identifier:
            case TokenType_StringV:
                printf("\"%s\"", token.literal->str);
                break;
            case TokenType_CharV:
                printf("'%c'", token.literal->ch);
                break;
            case TokenType_BoolV:
                printf("%s", token.literal->b ? "true" : "false");
                break;
            default:
                printf("()");
                break;
        }

        if (i + 1 < tokens.len) {
            printf(", ");
        }
    }
    printf("\n");

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

