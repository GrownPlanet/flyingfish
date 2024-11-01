#include <stdio.h>
#include <stdlib.h>

#include "file_utils.h"
#include "scanner.h"
#include "token.h"
#include "error.h"

void print_token(TokenType_t token);

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
            case IntV:
                printf("%ld", token.literal->ln);
                break;
            case FloatV:
                printf("%f", token.literal->db);
                break;
            case Identifier:
            case StringV:
                printf("\"%s\"", token.literal->str);
                break;
            case CharV:
                printf("'%c'", token.literal->ch);
                break;
            case BoolV:
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
    free_error();
    for (size_t i = 0; i < tokens.len; i++) {
        Token_t token = tokens.tokens[i];
        if (token.type == StringV || token.type == Identifier) {
            free(token.literal->str);
        }
    }

    return 0;
}

