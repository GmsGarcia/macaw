#include "buffer.h"
#include "editor.h"
#include <stdio.h>
#include <stdlib.h>

void load_buf(char *filename) {
  FILE *file = fopen(filename, "r");

  if (file) {
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (length > 0) {
      f_buffer = malloc(length);
      if (f_buffer) {
        fread(f_buffer, 1, length, file);
      }
    } else {
      gen_new_buf();
    }
    fclose(file);
  } else {
    gen_new_buf();
  }
}

void gen_new_buf() {
  f_buffer = malloc(1);
  if (f_buffer) {
    f_buffer[0] = '\0';
  }
}

void save_buf(char *filename, char *buffer) {}
