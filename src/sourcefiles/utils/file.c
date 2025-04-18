#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils/file.h"

String_t read_file_to_string(char* filename, char* mode) {
    FILE* file = fopen(filename, mode);

    if (file == NULL) {
        return (String_t) {
            .chars = NULL,
            .len = 0,
        };
    }

    fseek(file, 0L, SEEK_END);
    long file_len = ftell(file);
    rewind(file);

    char* file_data = (char*)malloc(sizeof(char) * (file_len + 1));

    if (file_data == NULL) {
        printf("malloc failed!\n");
        return (String_t) {
            .chars = NULL,
            .len = 0,
        };
    }

    size_t result = fread(file_data, 1, file_len, file);
    if (result != (size_t)file_len) {
        return (String_t) {
            .chars = NULL,
            .len = 0,
        };
    }

    file_data[file_len] = '\0';

    fclose(file);

    return (String_t) {
        .chars = file_data,
        .len = result,
    };
}
