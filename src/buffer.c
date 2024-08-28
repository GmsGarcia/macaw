#include "buffer.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_buf(DynamicBuffer *buf) {
  buf->size = 0;
  buf->length = 0;
  buf->data = malloc(buf->size);

  if (!buf->data) {
    endwin();
    printf("Failed to allocate initial buffer\n");
    exit(EXIT_FAILURE);
  }
}

void append_to_buf(DynamicBuffer *buf, const char *text, size_t len) {
  if (buf->length + len > buf->size) {
    while (buf->length + len > buf->size) {
      buf->size += BUFFER_SIZE_INCREMENT;
    }

    buf->data = realloc(buf->data, buf->size);
    if (!buf->data) {
      endwin();
      printf("Failed to reallocate memory for buffer\n");
      exit(EXIT_FAILURE);
    }
  }
  memcpy(buf->data + buf->length, text, len);
  buf->length += len;
}

void free_buf(DynamicBuffer *buf) {
  free(buf->data);
  buf->data = NULL;
  buf->size = 0;
  buf->length = 0;
}

void gen_new_buf(DynamicBuffer *buf) { init_buf(buf); }

void read_file_to_buf(char *filename, DynamicBuffer *buf) {
  FILE *file = fopen(filename, "rb");

  if (file) {
    init_buf(buf);

    char temp[INITIAL_BUFFER_SIZE];
    size_t bytes_read;

    while ((bytes_read = fread(temp, 1, sizeof(temp), file)) > 0) {
      append_to_buf(buf, temp, bytes_read);
    }

    fclose(file);
  } else {
    gen_new_buf(buf);
  }
}

void write_buf_to_file(char *filename, DynamicBuffer *buf) {}
