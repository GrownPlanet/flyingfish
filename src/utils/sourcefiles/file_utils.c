#include "file_utils.h"

char* read_to_string(char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    printf("Failed to open file!\n");
    exit(1);
  }

  fseek(file, 0L, SEEK_END);
  long file_len = ftell(file);
  rewind(file);

  char* file_data = (char*)malloc(sizeof(char) * (file_len + 1));

  if (file_data == NULL) {
    printf("malloc failed!\n");
    exit(1);
  }

  size_t result = fread(file_data, 1, file_len, file);

  if (result != (size_t)file_len) {
    printf("Error reading file!\n");
    exit(1);
  }

  file_data[file_len] = '\0';

  fclose(file);

  return file_data;
}
