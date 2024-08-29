#include "input.h"
#include "buffer.h"
#include "editor.h"
#include "render.h"
#include <ctype.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ctrl(x) (x & 0x1F)

void insert_char_to_buf(DynamicBuffer *buf, int x, int y, char c) {
  // Get the line from the buffer
  // Assuming buf->data is a string with newline characters separating lines
  char *line_start = buf->data;
  char *line_end = strchr(line_start, '\n');
  int line_number = y;
  while (line_end && line_number > 0) {
    line_start = line_end + 1;
    line_end = strchr(line_start, '\n');
    line_number--;
  }

  if (line_end) {
    size_t line_length = line_end - line_start;
    if (x > (int)line_length) {
      x = (int)line_length; // Clamp x to the end of the line
    }
  } else {
    // If there's no newline, we're at the end of the buffer
    line_end = buf->data + buf->length;
  }

  // Insert the character into the buffer
  // We will need to shift characters after the insertion point to make space
  size_t insertion_pos = line_start - buf->data + x;
  while (buf->length + 1 > buf->size) {
    buf->size += BUFFER_SIZE_INCREMENT;
  }
  memmove(buf->data + insertion_pos + 1, buf->data + insertion_pos,
          buf->length - insertion_pos);
  buf->data[insertion_pos] = c;
  buf->length++;
}

void delete_char_left_from_buf(DynamicBuffer *buf, int x, int y) {
  char *line_start = buf->data;
  char *line_end = strchr(line_start, '\n');
  int line_number = y;
  while (line_end && line_number > 0) {
    line_start = line_end + 1;
    line_end = strchr(line_start, '\n');
    line_number--;
  }

  if (line_end) {
    size_t line_length = line_end - line_start;
    if (x >= (int)line_length) {
      x = (int)line_length - 1;
    }
  } else {
    line_end = buf->data + buf->length;
  }

  size_t deletion_pos = line_start - buf->data + x;

  if (deletion_pos > 0) {
    memmove(buf->data + deletion_pos - 1, buf->data + deletion_pos,
            buf->length - deletion_pos);
    buf->length--;
  }
}

void delete_char_right_from_buf(DynamicBuffer *buf, int x, int y) {
  char *line_start = buf->data;
  char *line_end = strchr(line_start, '\n');
  int line_number = y;
  while (line_end && line_number > 0) {
    line_start = line_end + 1;
    line_end = strchr(line_start, '\n');
    line_number--;
  }

  if (line_end) {
    size_t line_length = line_end - line_start;
    if (x >= (int)line_length) {
      x = (int)line_length - 1;
    }
  } else {
    line_end = buf->data + buf->length;
  }

  size_t deletion_pos = line_start - buf->data + x;

  if (deletion_pos < buf->length) {
    memmove(buf->data + deletion_pos, buf->data + deletion_pos + 1,
            buf->length - deletion_pos - 1);
    buf->length--;
  }
}

void remove_line_from_buf(DynamicBuffer *buf, int y) {
  char *line_start = buf->data;
  char *line_end = strchr(line_start, '\n');
  int line_number = y;

  while (line_end && line_number > 0) {
    line_start = line_end + 1;
    line_end = strchr(line_start, '\n');
    line_number--;
  }

  if (line_end) {
    char *next_line_start = line_end + 1;
    size_t line_length = next_line_start - line_start - 1;

    memmove(line_start, next_line_start,
            buf->length - (next_line_start - buf->data));
    buf->length -= line_length + 1; // Adjust length for the newline character
  } else {
    buf->length = line_start - buf->data;
  }

  // Null-terminate the buffer
  buf->data[buf->length] = '\0';
}

void handle_input() {
  int c = wgetch(m_win);

  if (isalpha(c)) {
    c = tolower(c);
  }

  switch (mode) {
  case NORMAL:
    switch (c) {
    case KEY_LEFT:
    case 'h':
      if (cur_x > 0) {
        cur_x--;
      }
      break;
    case KEY_DOWN:
    case 'j':
      // buf size
      if (cur_y < get_buf_height()) {
        cur_y++;
        int c_len = get_line_length(cur_y);
        if (cur_x > c_len) {
          cur_x = c_len;
        }
      }
      break;
    case KEY_UP:
    case 'k':
      if (cur_y > 0) {
        cur_y--;
        int c_len = get_line_length(cur_y);
        if (cur_x > c_len) {
          cur_x = c_len;
        }
      }
      break;
    case KEY_RIGHT:
    case 'l':
      if (cur_x < get_line_length(cur_y)) {
        cur_x++;
      }
      break;
    case 'i':
      mode = INSERT;
      break;
    case 'a':
      cur_x++;
      mode = INSERT;
      break;
    case ctrl('d'):
      remove_line_from_buf(&f_buf, cur_y);
      break;
    case ':':
      mode = COMMAND;
      curs_set(0);
      keypad(stdscr, FALSE);
      keypad(m_win, FALSE);
      memset(message, 0, sizeof(message));
      break;
    }
    break;
  case INSERT:
    switch (c) {
    case 27:
    case ctrl('c'):
      mode = NORMAL;
      break;
    case KEY_BACKSPACE:
      int prev_line = get_line_length(cur_y - 1);
      delete_char_left_from_buf(&f_buf, cur_x, cur_y);
      if (cur_x <= 0) {
        cur_y -= 1;
        cur_x = prev_line;
      } else {
        cur_x--;
      }
      break;
    case KEY_DC:
      delete_char_right_from_buf(&f_buf, cur_x, cur_y);
      break;

    case 10:
      insert_char_to_buf(&f_buf, cur_x, cur_y, c);
      cur_y += 1;
      cur_x = 0;
      break;
    default:
      insert_char_to_buf(&f_buf, cur_x, cur_y, c);
      cur_x++;
      break;
    }
    break;
  case COMMAND:
    // ESC or CTRL-c
    if (c == 27 || c == ctrl('c')) {
      curs_set(2);
      mode = NORMAL;
      memset(command, 0, sizeof(command));

      // CARRIAGE RETURN
    } else if (c == 10) {
      if (strcmp(command, "q") == 0) {
        endwin();
        exit(0);
      } else if (strcmp(command, "w") == 0) {
        write_buf_to_file(&f_buf);
        sprintf(message, "Saved file: %s, %dL, %zuB", f_name,
                get_buf_height() + 1, f_buf.size);
        memset(command, 0, sizeof(command));
      } else if (strcmp(command, "wq") == 0) {
        write_buf_to_file(&f_buf);
        endwin();
        exit(0);
      } else {
        sprintf(message, "Not an editor command: %s", command);
        memset(command, 0, sizeof(command));
      }

      keypad(stdscr, TRUE);
      keypad(m_win, TRUE);

      // BACKSPACE or DELETE
    } else if (c == 8 || c == 127) {
      command[strlen(command) - 1] = '\0';
    } else {
      char cha[2];
      cha[0] = c;
      cha[1] = '\0';
      strncat(command, cha, 1);
    }
    break;
  }

  wmove(m_win, cur_y, cur_x);
}
