#include "buffer.h"
#include "editor.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_buf(DynamicBuffer *buf) {
  buf->size = 0;
  buf->data = malloc(buf->size * sizeof(char *));
  if (!buf->data) {
    endwin();
    printf("Failed to allocate memory for buffer\n");
    exit(EXIT_FAILURE);
  }
}

void free_buf(DynamicBuffer *buf) {
  // free every line
  for (int i = 0; i < buf->size; i++) {
    free(buf->data[i]);
  }
  // free array of pointers
  free(buf->data);

  // nullify fields
  buf->data = NULL;
  buf->size = 0;
}

void read_line_to_buf(DynamicBuffer *buf, const char *text) {
  char *start = (char *)text;
  char *end = NULL;

  // while don't found new line
  while ((end = strchr(start, '\n')) != NULL) {
    size_t line_len = end - start + 1;

    // grow array by 1
    buf->size += 1;
    buf->data = realloc(buf->data, buf->size * sizeof(char *));

    if (!buf->data) {
      endwin();
      printf("Failed to reallocate memory for buffer\n");
      exit(EXIT_FAILURE);
    }

    buf->data[buf->size - 1] = malloc((line_len + 1) * sizeof(char *));
    if (!buf->data[buf->size - 1]) {
      endwin();
      printf("Failed to allocate memory for buffer\n");
      exit(EXIT_FAILURE);
    }
    memcpy(buf->data[buf->size - 1], start, line_len);
    buf->data[buf->size - 1][line_len] = '\0';

    start = end + 1;
  }
}

void read_file_to_buf(DynamicBuffer *buf, char *path) {
  FILE *file = fopen(path, "rb");

  if (file) {
    char temp[INITIAL_BUFFER_SIZE];
    while (fgets(temp, sizeof(temp), file)) {
      read_line_to_buf(buf, temp);
    }
    fclose(file);
  } else {
    sprintf(message, "Failed to open file for reading: %s", path);
  }
}

void write_buf_to_file(DynamicBuffer *buf, char *path) {
  FILE *file = fopen(path, "wb");

  if (file) {
    for (size_t i = 0; i < buf->size - 1; i++) {
      fputs(buf->data[i], file);
    }
    fseek(file, 0, SEEK_END);
    buf->bytes = ftell(file);
    fclose(file);
  } else {
    sprintf(message, "Failed to open file for writing: %s", path);
  }
}

void insert_char_to_buf(DynamicBuffer *buf, int cur_x, int cur_y, char c) {}

void insert_new_line_to_buf(DynamicBuffer *buf, int cur_x, int cur_y) {}

void remove_prev_char_from_buf(DynamicBuffer *buf, int cur_x, int cur_y) {}

void remove_cur_char_from_buf(DynamicBuffer *buf, int curcur__x, int cur_y) {}

void remove_line_from_buf(DynamicBuffer *buf, int cur_y) {
  if (cur_y >= buf->size) {
    return; // Line number is out of range
  }

  free(buf->data[cur_y]);

  if (buf->size > 1) {
    for (size_t i = cur_y; i < buf->size - 1; i++) {
      buf->data[i] = buf->data[i + 1];
    }

    buf->size -= 1;
    buf->data = realloc(buf->data, buf->size * sizeof(char *));

    if (!buf->data) {
      endwin();
      printf("Failed to reallocate memory for buffer\n");
      exit(EXIT_FAILURE);
    }
  } else {
    buf->data[cur_y] = malloc(1 * sizeof(char *));
    buf->data[cur_y][0] = '\0';
  }
}

void add_line_to_buf(DynamicBuffer *buf, int cur_y) {
  if (cur_y >= buf->size) {
    return; // Line number is out of range
  }

  buf->size += 1;
  buf->data = realloc(buf->data, buf->size * sizeof(char *));

  for (size_t i = buf->size - 1; i > cur_y; i--) {
    buf->data[i] = buf->data[i - 1];
  }

  if (!buf->data) {
    endwin();
    printf("Failed to reallocate memory for buffer\n");
    exit(EXIT_FAILURE);
  }

  buf->data[cur_y + 1] = malloc(1 * sizeof(char *));
  buf->data[cur_y + 1][0] = '\0';
}

int get_line_length(int cur_y) { return strlen(f_buf.data[cur_y]); }
