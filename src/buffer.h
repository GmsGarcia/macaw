#ifndef BUFFER_H
#define BUFFER_H

#define INITIAL_BUFFER_SIZE 1024
#define BUFFER_SIZE_INCREMENT 1024

#include <stdio.h>

typedef struct {
  char *data;
  size_t size;
  size_t length;
} DynamicBuffer;

extern char *filename;

void init_buf(DynamicBuffer *buf);

void append_to_buf(DynamicBuffer *buf, const char *text, size_t len);

void free_buf(DynamicBuffer *buf);

void gen_new_buf(DynamicBuffer *buf);

void read_file_to_buf(char *filename, DynamicBuffer *buf);

void write_buf_to_file(DynamicBuffer *buf);

int get_buf_height();

int get_line_length(int cur_x);

#endif
