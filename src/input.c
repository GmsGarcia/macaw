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

void handle_input() {
  int c = wgetch(m_win);
  int c_len = get_line_length(cur_y) - 2;

  if (c_len <= 0) {
    c_len = 0;
  }

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
      if (cur_y < f_buf.size - 1) {
        cur_y++;
        c_len = get_line_length(cur_y) - 2;
        if (cur_x > c_len) {
          cur_x = c_len;
        }
        if (c_len <= 0) {
          cur_x = 0;
        }
      }
      break;
    case KEY_UP:
    case 'k':
      if (cur_y > 0) {
        cur_y--;
        c_len = get_line_length(cur_y) - 2;
        if (cur_x > c_len) {
          cur_x = c_len;
        }
        if (c_len <= 0) {
          cur_x = 0;
        }
      }
      break;
    case KEY_RIGHT:
    case 'l':
      if (cur_x < c_len) {
        cur_x++;
      }
      break;
      // switch to 0
    case 'b':
      cur_x = 0;
      break;
      // switch to $
    case 'w':
      cur_x = c_len;
      break;
    case 'i':
      mode = INSERT;
      break;
    case 'a':
      cur_x++;
      mode = INSERT;
      break;
    case 'o':
      mode = INSERT;
      add_line_to_buf(&f_buf, cur_y);
      cur_y += 1;
      cur_x = 0;
      break;
    case 'x':
      delete_cur_char_from_buf(&f_buf, cur_x, cur_y);
      break;
    case ctrl('d'):
      remove_line_from_buf(&f_buf, cur_y);
      break;
    case ':':
      mode = COMMAND;
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
      cur_x--;
      break;
    case KEY_BACKSPACE:
      int prev_line = get_line_length(cur_y - 1) - 2;
      delete_prev_char_from_buf(&f_buf, cur_x, cur_y);
      if (cur_x <= 0) {
        cur_y -= 1;
        cur_x = prev_line;
      } else {
        cur_x--;
      }
      break;
    case KEY_DC:
      delete_cur_char_from_buf(&f_buf, cur_x, cur_y);
      break;
    case 10:
      insert_new_line_to_buf(&f_buf, cur_x, cur_y);
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
      mode = NORMAL;
      memset(command, 0, sizeof(command));
      // CARRIAGE RETURN
    } else if (c == 10) {
      if (strcmp(command, "q") == 0) {
        endwin();
        exit(0);
      } else if (strcmp(command, "w") == 0) {
        write_buf_to_file(&f_buf, f_name);
        sprintf(message, "\"%s\" %zuL, %zuB written", f_name, f_buf.size,
                f_buf.bytes);
        memset(command, 0, sizeof(command));
      } else if (strcmp(command, "wq") == 0) {
        write_buf_to_file(&f_buf, f_name);
        endwin();
        exit(0);
      } else {
        sprintf(message, "Not an editor command: %s", command);
        memset(command, 0, sizeof(command));
      }

      keypad(stdscr, TRUE);
      keypad(m_win, TRUE);

      mode = NORMAL;
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
