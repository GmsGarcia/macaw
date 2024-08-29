#include "buffer.h"
#include "editor.h"
#include "render.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *filename;

void init_buf(DynamicBuffer *buf) {
  buf->size = BUFFER_SIZE_INCREMENT;
  buf->length = 0;
  buf->data = malloc(buf->size);

  if (!buf->data) {
    endwin();
    printf("Failed to allocate initial buffer\n");
    exit(EXIT_FAILURE);
  }
  buf->data[0] = '\0';
}

void append_to_buf(DynamicBuffer *buf, const char *text, size_t len) {
  if (buf->length + len >= buf->size) {
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

  buf->data[buf->length] = '\0';
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

void write_buf_to_file(DynamicBuffer *buf) {
  FILE *file = fopen(f_name, "wb");

  if (file) {
    fwrite(buf->data, 1, buf->length, file);
    fclose(file);
  } else {
    sprintf(message, "Failed to open file for writing: %s", f_name);
  }
}

int get_buf_height() {
  int y = 0, x = 0; // Start rendering from (1, 1) to avoid border overlap
  int max_y = m_max_height;
  int max_x = m_max_width;
  int current_line_length = 0;

  for (size_t i = 0; i < f_buf.length; i++) {
    char ch = f_buf.data[i];

    // Handle newlines and reset position
    if (ch == '\n' || current_line_length >= max_x) {
      y++;
      x = 0;
      current_line_length = 0;
    } else {
      x++;
      current_line_length++;
    }

    // Stop rendering if we reach the bottom of the window
    if (y >= max_y) {
      break;
    }
  }

  return y - 1;
}

int get_line_length(int cur_y) {
  char *line_start = f_buf.data;
  char *line_end = strchr(line_start, '\n');
  int current_line = 0;

  while (line_end && current_line < cur_y) {
    line_start = line_end + 1;
    line_end = strchr(line_start, '\n');
    current_line++;
  }

  if (line_end) {
    return line_end - line_start;
  } else {
    return strlen(line_start); // If there's no newline, return the rest of the
                               // buffer length
  }
}
