#include "buffer.h"
#include "editor.h"
#include "render.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_buf(DynamicBuffer *buf) {
  buf->size = BUFFER_SIZE_INCREMENT;
  buf->line_count = 0;
  buf->data = malloc(buf->size * sizeof(char *));
  buf->bytes = 0;

  if (!buf->data) {
    endwin();
    printf("Failed to allocate initial buffer\n");
    exit(EXIT_FAILURE);
  }
}

void append_to_buf(DynamicBuffer *buf, const char *text, size_t len) {
  char *start = (char *)text;
  char *end = NULL;

  while ((end = strchr(start, '\n')) != NULL) {
    size_t line_len = end - start + 1; // Include the newline character

    if (buf->line_count >= buf->size) {
      buf->size += BUFFER_SIZE_INCREMENT;
      buf->data = realloc(buf->data, buf->size * sizeof(char *));
      if (!buf->data) {
        endwin();
        printf("Failed to reallocate memory for buffer\n");
        exit(EXIT_FAILURE);
      }
    }

    buf->data[buf->line_count] = malloc(line_len + 1);
    if (!buf->data[buf->line_count]) {
      endwin();
      printf("Failed to allocate memory for line\n");
      exit(EXIT_FAILURE);
    }

    memcpy(buf->data[buf->line_count], start, line_len);
    buf->data[buf->line_count][line_len] = '\0';

    buf->line_count++;
    start = end + 1; // Move to the start of the next line
  }

  // Handle any remaining text (if the last part didn't end with a newline)
  if (*start != '\0') {
    size_t line_len = strlen(start);
    if (buf->line_count >= buf->size) {
      buf->size += BUFFER_SIZE_INCREMENT;
      buf->data = realloc(buf->data, buf->size * sizeof(char *));
      if (!buf->data) {
        endwin();
        printf("Failed to reallocate memory for buffer\n");
        exit(EXIT_FAILURE);
      }
    }

    buf->data[buf->line_count] = malloc(line_len + 1);
    if (!buf->data[buf->line_count]) {
      endwin();
      printf("Failed to allocate memory for line\n");
      exit(EXIT_FAILURE);
    }

    memcpy(buf->data[buf->line_count], start, line_len);
    buf->data[buf->line_count][line_len] = '\0';

    buf->line_count++;
  }
}

void free_buf(DynamicBuffer *buf) {
  for (size_t i = 0; i < buf->line_count; i++) {
    free(buf->data[i]);
  }
  free(buf->data);
  buf->data = NULL;
  buf->size = 0;
  buf->line_count = 0;
}

void gen_new_buf(DynamicBuffer *buf) { init_buf(buf); }

void read_file_to_buf(char *path, DynamicBuffer *buf) {
  filename = strrchr(path, '/') + 1;
  FILE *file = fopen(path, "rb");

  if (file) {
    init_buf(buf);

    char temp[INITIAL_BUFFER_SIZE];
    while (fgets(temp, sizeof(temp), file)) {
      append_to_buf(buf, temp, strlen(temp));
    }

    fclose(file);
  } else {
    gen_new_buf(buf);
  }
}

void write_buf_to_file(DynamicBuffer *buf, const char *f_name) {
  FILE *file = fopen(f_name, "wb");

  if (file) {
    for (size_t i = 0; i < buf->line_count; i++) {
      fputs(buf->data[i], file);
    }
    fseek(file, 0, SEEK_END);
    buf->bytes = ftell(file);
    fclose(file);
  } else {
    printf("Failed to open file for writing: %s\n", f_name);
  }
}

int get_buf_height() { return f_buf.line_count; }

int get_line_length(int cur_y) {
  if (cur_y < 0 || cur_y >= f_buf.line_count) {
    return 0; // Out of bounds
  }
  return strlen(f_buf.data[cur_y]);
}

void insert_char_to_buf(DynamicBuffer *buf, int x, int y, char c) {
  if (y >= buf->line_count) {
    // Line number is out of range; append new lines if necessary
    while (y >= buf->line_count) {
      // Add empty lines to accommodate the new line insertion
      buf->data[buf->line_count] =
          malloc(1); // Allocate space for an empty line
      if (!buf->data[buf->line_count]) {
        endwin();
        printf("Failed to allocate memory for new line\n");
        exit(EXIT_FAILURE);
      }
      buf->data[buf->line_count][0] = '\0';
      buf->line_count++;
    }
  }

  // Get the line to modify
  char *line = buf->data[y];
  size_t line_length = strlen(line);

  // Adjust x if it's beyond the current length of the line
  if (x > (int)line_length) {
    x = (int)line_length; // Clamp x to the end of the line
  }

  // Reallocate memory if necessary
  size_t new_length =
      line_length + 2; // +1 for the new char, +1 for the null terminator
  char *new_line = realloc(line, new_length);
  if (!new_line) {
    endwin();
    printf("Failed to reallocate memory for line\n");
    exit(EXIT_FAILURE);
  }

  // Shift characters to make space for the new character
  memmove(new_line + x + 1, new_line + x, line_length - x + 1);
  new_line[x] = c;         // Insert the new character
  buf->data[y] = new_line; // Update the buffer's line pointer

  buf->line_count =
      y + 1; // Ensure line_count is updated if a new line was added
}

void delete_char_left_from_buf(DynamicBuffer *buf, int x, int y) {
  if (y >= buf->line_count) {
    return; // Line number is out of range
  }

  // Get the line to modify
  char *line = buf->data[y];
  size_t line_length = strlen(line);

  // Adjust x if it's beyond the current length of the line
  if (x >= (int)line_length) {
    x = (int)line_length - 1;
  }

  if (x > 0) {
    // Shift characters to the left
    memmove(line + x - 1, line + x, line_length - x + 1);
    // Reallocate memory if needed
    char *new_line = realloc(line, line_length);
    if (!new_line) {
      endwin();
      printf("Failed to reallocate memory for line\n");
      exit(EXIT_FAILURE);
    }
    buf->data[y] = new_line;
  } else {
    // If x is 0, simply remove the first character
    size_t new_length = line_length - 1;
    char *new_line = realloc(line, new_length + 1);
    if (!new_line) {
      endwin();
      printf("Failed to reallocate memory for line\n");
      exit(EXIT_FAILURE);
    }
    memmove(new_line, new_line + 1, new_length);
    new_line[new_length] = '\0';
    buf->data[y] = new_line;
  }
}

void delete_char_right_from_buf(DynamicBuffer *buf, int x, int y) {
  if (y >= buf->line_count) {
    return; // Line number is out of range
  }

  // Get the line to modify
  char *line = buf->data[y];
  size_t line_length = strlen(line);

  // Adjust x if it's beyond the current length of the line
  if (x >= (int)line_length) {
    x = (int)line_length - 1;
  }

  // Shift characters to the left
  if (x < (int)line_length - 1) {
    memmove(line + x, line + x + 1, line_length - x);
    // Reallocate memory if needed
    char *new_line = realloc(line, line_length);
    if (!new_line) {
      endwin();
      printf("Failed to reallocate memory for line\n");
      exit(EXIT_FAILURE);
    }
    buf->data[y] = new_line;
  } else {
    // If x is the last character, simply reduce the length
    size_t new_length = line_length - 1;
    char *new_line = realloc(line, new_length + 1);
    if (!new_line) {
      endwin();
      printf("Failed to reallocate memory for line\n");
      exit(EXIT_FAILURE);
    }
    new_line[new_length] = '\0';
    buf->data[y] = new_line;
  }
}

void remove_line_from_buf(DynamicBuffer *buf, int y) {
  if (y >= buf->line_count) {
    return; // Line number is out of range
  }

  // Free the line to be removed
  free(buf->data[y]);

  // Move subsequent lines up
  for (size_t i = y; i < buf->line_count - 1; i++) {
    buf->data[i] = buf->data[i + 1];
  }

  // Reduce line count
  buf->line_count--;

  // Null-terminate the buffer
  buf->data[buf->line_count] = NULL;
}
