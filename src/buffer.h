#ifndef BUFFER_H
extern char *filename;

#define BUFFER_H

#define INITIAL_BUFFER_SIZE 1024
#define BUFFER_SIZE_INCREMENT 1

#include <stdio.h>

typedef struct {
  char **data;
  size_t size;
  long bytes;
} DynamicBuffer;

void init_buf(DynamicBuffer *buf);

void append_to_buf(DynamicBuffer *buf, const char *text);

void free_buf(DynamicBuffer *buf);

void gen_new_buf(DynamicBuffer *buf);

void read_file_to_buf(DynamicBuffer *buf, char *path);

void write_buf_to_file(DynamicBuffer *buf, char *path);

void insert_char_to_buf(DynamicBuffer *buf, int cur_x, int cur_y, char c);

void insert_new_line_to_buf(DynamicBuffer *buf, int cur_x, int cur_y);

void delete_prev_char_from_buf(DynamicBuffer *buf, int cur_x, int cur_y);

void delete_cur_char_from_buf(DynamicBuffer *buf, int cur_x, int cur_y);

void remove_line_from_buf(DynamicBuffer *buf, int cur_y);

void add_line_to_buf(DynamicBuffer *buf, int cur_y);

int get_line_length(int cur_y);

#endif
