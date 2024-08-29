#ifndef BUFFER_H
#define BUFFER_H

#define INITIAL_BUFFER_SIZE 1024
#define BUFFER_SIZE_INCREMENT 1

#include <stdio.h>

typedef struct {
  char **data;
  size_t line_count;
  size_t size;
  long bytes;
} DynamicBuffer;

extern char *filename;

void init_buf(DynamicBuffer *buf);

void append_to_buf(DynamicBuffer *buf, const char *text, size_t len);

void free_buf(DynamicBuffer *buf);

void gen_new_buf(DynamicBuffer *buf);

void read_file_to_buf(char *filename, DynamicBuffer *buf);

void write_buf_to_file(DynamicBuffer *buf, const char *f_name);

void insert_char_to_buf(DynamicBuffer *buf, int x, int y, char c);

void delete_char_left_from_buf(DynamicBuffer *buf, int x, int y);

void delete_char_right_from_buf(DynamicBuffer *buf, int x, int y);

void remove_line_from_buf(DynamicBuffer *buf, int y);

int get_buf_height();

int get_line_length(int cur_x);

#endif
