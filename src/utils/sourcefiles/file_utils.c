#include "file_utils.h"

char* read_file_to_string(char* filename) {
  FILE* file = fopen(filename, "r");

  if (file == NULL) {
    printf("Failed to open file!\n");
    return NULL;
  }

  fseek(file, 0L, SEEK_END);
  long file_len = ftell(file);
  rewind(file);

  char* file_data = (char*)malloc(sizeof(char) * (file_len + 1));

  if (file_data == NULL) {
    printf("malloc failed!\n");
    return NULL;
  }

  size_t result = fread(file_data, 1, file_len, file);

  if (result != (size_t)file_len) {
    printf("Error reading file!\n");
    return NULL;
  }

  file_data[file_len] = '\0';

  fclose(file);

  return file_data;
}
