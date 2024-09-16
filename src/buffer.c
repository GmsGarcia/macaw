#include "buffer.h"
#include "editor.h"
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_buf(DynamicBuffer *buf) {
  buf->size = 1;
  buf->data = malloc(buf->size * sizeof(char *));
  if (!buf->data) {
    endwin();
    printf("Failed to allocate memory for buffer\n");
    exit(EXIT_FAILURE);
  }

  buf->data[buf->size - 1] = malloc(2 * sizeof(char));
  buf->data[buf->size - 1][0] = '\n';
  buf->data[buf->size - 1][1] = '\0';
}


void free_buf(DynamicBuffer *buf) {
  for (int i = 0; i < buf->size; i++) {
    free(buf->data[i]);
  }
  free(buf->data);

  buf->data = NULL;
  buf->size = 0;
}

void read_line_to_buf(DynamicBuffer *buf, const char *text) {
  char *start = (char *)text;
  char *end = NULL;

  while ((end = strchr(start, '\n')) != NULL) {
    size_t line_len = end - start + 1;

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
    char line[INITIAL_BUFFER_SIZE];

    buf->size = 0;
    buf->data = realloc(buf->data, buf->size * sizeof(char *));

    while (fgets(line, sizeof(line), file)) {
      read_line_to_buf(buf, line);
    }
    fclose(file);
  } else {
    sprintf(message, "Failed to open file for reading: %s", path);
  }
}

void write_buf_to_file(DynamicBuffer *buf, char *path) {
  FILE *file = fopen(path, "wb");

  if (file) {
    for (size_t i = 0; i < buf->size; i++) {
      fputs(buf->data[i], file);
    }

    fseek(file, 0, SEEK_END);
    buf->bytes = ftell(file);
    fclose(file);

    sprintf(message, "\"%s\" %zuL, %zuB written", f_name, f_buf.size, f_buf.bytes);
  } else {
    sprintf(message, "Failed to open file for writing: %s", path);
  }
}

void insert_char_to_buf(DynamicBuffer *buf, int cur_x, int cur_y, char c) {
  int len = strlen(f_buf.data[cur_y]) + 1;
  buf->data[cur_y] = realloc(buf->data[cur_y], len + 1);

  for (int i = len; i > cur_x; i--) {
    buf->data[cur_y][i] = buf->data[cur_y][i - 1];
  }
  buf->data[cur_y][cur_x] = c;
  buf->data[cur_y][len-1] = '\n';
  buf->data[cur_y][len] = '\0';
}

void insert_new_line_char_to_buf(DynamicBuffer *buf, int cur_x, int cur_y) {
  // reallocate one more pointer
  buf->size += 1;
  buf->data = realloc(buf->data, buf->size * sizeof(char *));

  if (!buf->data) {
    endwin();
    printf("Failed to reallocate memory for buffer\n");
    exit(EXIT_FAILURE);
  }

  // shifting lines
  for (size_t i = buf->size - 1; i > cur_y; i--) {
    buf->data[i] = buf->data[i - 1];
  }

  // allocating next line n bytes
  int cur_line_len = strlen(buf->data[cur_y]);
  int next_line_len = cur_line_len - cur_x;

  buf->data[cur_y + 1] = (char *)malloc((next_line_len + 2) * sizeof(char));

  if (!buf->data[cur_y + 1]) {
    endwin();
    printf("Failed to allocate memory for buffer\n");
    exit(EXIT_FAILURE);
  }

  // copying data from current to next line starting at cur_x position
  strncpy(buf->data[cur_y + 1], &buf->data[cur_y][cur_x], next_line_len);
  buf->data[cur_y + 1][next_line_len - 1] = '\n';
  buf->data[cur_y + 1][next_line_len] = '\0';

  // reallocating current line to shrink data
  buf->data[cur_y] = realloc(buf->data[cur_y], (cur_x + 2) * sizeof(char));

  if (!buf->data[cur_y]) {
    endwin();
    printf("Failed to reallocate memory for buffer\n");
    exit(EXIT_FAILURE);
  }

  // setting new lines and null characters
  buf->data[cur_y][cur_x] = '\n';
  buf->data[cur_y][cur_x + 1] = '\0';
}

void remove_prev_char_from_buf(DynamicBuffer *buf, int cur_x, int cur_y) {
  int cur_line_len = strlen(buf->data[cur_y]);

  if (cur_x > 0) {
    for (int i = cur_x - 1; i < cur_line_len; i++) {
      buf->data[cur_y][i] = buf->data[cur_y][i + 1];
    }

    buf->data[cur_y] = realloc(buf->data[cur_y], cur_line_len);
  } else {
    if (cur_line_len > 1) {
      int prev_line_len = strlen(buf->data[cur_y - 1]);
      int new_prev_line_len = strlen(buf->data[cur_y - 1]) + cur_line_len - 1;

      buf->data[cur_y - 1] = realloc(buf->data[cur_y - 1], new_prev_line_len);

      for (int i = 0; i < cur_line_len; i++) {
        buf->data[cur_y - 1][prev_line_len - 1 + i] = buf->data[cur_y][i];
      }
    }

    remove_line_from_buf(buf, cur_y);
  }
}

void remove_cur_char_from_buf(DynamicBuffer *buf, int cur_x, int cur_y) {
  int cur_line_len = strlen(buf->data[cur_y]);

  if (cur_x < cur_line_len - 1) {
    for (int i = cur_x; i < cur_line_len; i++) {
      buf->data[cur_y][i] = buf->data[cur_y][i + 1];
    }

    buf->data[cur_y] = realloc(buf->data[cur_y], cur_line_len);
  } else {
    int next_line_len = strlen(buf->data[cur_y + 1]);
    if (next_line_len > 1) {
      int new_cur_line_len = cur_line_len + strlen(buf->data[cur_y + 1]) - 1;

      buf->data[cur_y] = realloc(buf->data[cur_y], new_cur_line_len);

      for (int i = 0; i < cur_line_len; i++) {
        buf->data[cur_y][cur_line_len - 1 + i] = buf->data[cur_y + 1][i];
      }
    }

    remove_line_from_buf(buf, cur_y+1);
  }
}

void remove_line_from_buf(DynamicBuffer *buf, int cur_y) {
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
    buf->data[cur_y] = malloc(2 * sizeof(char *));
    buf->data[cur_y][0] = '\n'; 
    buf->data[cur_y][1] = '\0';
  }
}

void add_line_to_buf(DynamicBuffer *buf, int cur_y) {
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

  buf->data[cur_y + 1] = malloc(2 * sizeof(char *));
  buf->data[cur_y + 1][0] = '\n';
  buf->data[cur_y + 1][1] = '\0';
}

int get_line_length(int cur_y) {
  if (cur_y >= 0 && cur_y < f_buf.size) {
    return strlen(f_buf.data[cur_y]);
  }
  return 0;
}

int can_go_to_x(int x, int y) {
  if (x >= 0 && x < get_line_length(y) - 2) {
    return 1;
  }
  return 0;
}

int can_go_to_y(int y) {
  if (y >= 0 && y <= f_buf.size - 1) {
    return 1;
  } 
  return 0;
}
