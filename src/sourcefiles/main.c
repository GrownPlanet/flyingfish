#include <stdio.h>
#include <stdlib.h>

#include "file_utils.h"
#include "scanner.h"
#include "token.h"

void print_token(TokenType_t token);

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Ussage: %s {program file}.\n", argv[0]);
        return 1;
    }

    char* file = read_to_string(argv[1]);

    ScanResult_t tokens = scan(file, strlen(file));

    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        printf("%d: ", token.type);
        switch (token.type) {
            case Int:
                printf("%ld", token.literal->ln);
                break;
            case Float:
                printf("%f", token.literal->db);
                break;
            case Identifier:
            case String:
                printf("\"%s\"", token.literal->str);
                break;
            case Char:
                printf("'%c'", token.literal->ch);
                break;
            case Bool:
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

    return 0;
}

