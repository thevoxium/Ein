#include "utils.h"

char *read_ein_file(char *filename, long *out_length) {
  FILE *fp = fopen(filename, "rb");
  if (!fp)
    return NULL;

  if (fseek(fp, 0, SEEK_END) != 0) {
    fclose(fp);
    return NULL;
  }

  long size = ftell(fp);
  if (size < 0) {
    fclose(fp);
    return NULL;
  }

  rewind(fp);

  char *buffer = malloc(size + 1);
  if (!buffer) {
    fclose(fp);
    return NULL;
  }

  size_t read_bytes = fread(buffer, 1, size, fp);
  fclose(fp);

  if (read_bytes != (size_t)size) {
    free(buffer);
    return NULL;
  }

  buffer[size] = '\0';

  if (out_length)
    *out_length = size;

  return buffer;
}
