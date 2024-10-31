#include <stdio.h>
#include <stdlib.h>

#include "file_utils.h"
#include "scanner.h"
#include "token.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Ussage: %s {program file}.\n", argv[0]);
        return 1;
    }

    char* file = read_to_string(argv[1]);

    ScanResult_t tokens = scan(file, strlen(file));

    for (size_t i = 0; i < tokens.len; i++) {
        printf("%d, ", tokens.tokens[i].type);
    }
    printf("\n");

    return 0;
}
