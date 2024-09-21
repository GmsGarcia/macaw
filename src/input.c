#include "input.h"
#include "buffer.h"
#include "editor.h"
#include "render.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ctrl(x) (x & 0x1F)

int g_total_vlines = 0;
int g_cur_vline = 0;

void handle_input() {
  int c = wgetch(m_win);
  int c_len = get_line_length(cur_y) - 1;
  int prev_line;

  if (c_len <= 0) {
    c_len = 0;
  }

  switch (mode) {
  case NORMAL:
    switch (c) {
    case KEY_LEFT:
    case 'h':
      if (can_go_to_x(cur_x - 1, cur_y)) {
        cur_x--;
        saved_x = cur_x;
      }
      break;
    case KEY_DOWN:
    case 'j':
      // buf size
      if (can_go_to_y(cur_y + 1)) {
        cur_y_len = get_line_length(cur_y) - 2;

        int total_vlines = cur_y_len / m_max_width;
        int cur_vline = cur_x / m_max_width;

        // if line is wrapped, increase cur_x, otherwise just increase cur_y and clear cur_x
        if (cur_vline < total_vlines) {
          cur_x += m_max_width;
        } else {
          cur_x = cur_scr_x;
          cur_y++;
        }

        // scrolls the viewport if the cursor is at the border (+ padding)
        if (cur_scr_y >= vport_height - 10) {
          vport_start_y++;
        }
      } 
      break;
    case KEY_UP:
    case 'k':
      if (can_go_to_y(cur_y - 1)) {
        cur_y_len = get_line_length(cur_y) - 2;

        int cur_vline = cur_x / m_max_width;

        // if line is wrapped, decrease cur_x, otherwise just decrease cur_y and clear cur_x
        if (cur_vline > 0) {
          cur_x -= m_max_width;
        } else {
          cur_y--;

          cur_y_len = get_line_length(cur_y) - 2;
          int total_vlines = cur_y_len / m_max_width;

          if (total_vlines > 1) {
            cur_x = (total_vlines * m_max_width) + cur_scr_x;
          } else {
            cur_x = cur_scr_x;
          }
        }

        // scrolls the viewport if the cursor is at the border (+ padding)
        if (cur_scr_y < 10 && vport_start_y > 0) {
          vport_start_y--;
        }
      }
      break;
    case KEY_RIGHT:
    case 'l':
      if (can_go_to_x(cur_x, cur_y)) {
        cur_x++;
        saved_x = cur_x;
      }
      break;
      // switch to 0
    case 'b':
      cur_x = 0;
      saved_x = cur_x;
      break;
      // switch to $
    case 'w':
      cur_x = c_len - 1;
      saved_x = cur_x;
      break;
        case 'g':
          cur_y = 0;
          vport_start_y = 0;
        break;
        case 'G':
          cur_y = f_buf.size - 1;
          if (cur_y >= vport_height) {
            vport_start_y = cur_y - vport_height + 1;
          }
        break;
    case 'i':
      mode = INSERT;
      break;
    case 'a':
      if (get_line_length(cur_y) > 1) {
        cur_x++;
      } 
      mode = INSERT;
      break;
    case 'o':
      mode = INSERT;
      add_line_to_buf(&f_buf, cur_y);
      cur_y += 1;
      cur_x = 0;
      break;
    case 'O':
      mode = INSERT;
      add_line_to_buf(&f_buf, cur_y - 1);
      cur_x = 0;
      break;
    case 'x':
      if (c_len > 0) {
        remove_cur_char_from_buf(&f_buf, cur_x, cur_y);
        c_len = get_line_length(cur_y) - 1;
        if (cur_x >= c_len) {
          cur_x -= 1;
        }
      }
      break;
    case ctrl('d'):
          // empty line if is cur_y = 0
      remove_line_from_buf(&f_buf, cur_y);
      if (cur_y >= f_buf.size - 1 && cur_y > 0) {
        cur_y--;
      } else {
        cur_x = 0;
      }
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
      if (cur_x > 1) {
        cur_x--;
      }
      break;
      // TAB
    case 9:
      insert_char_to_buf(&f_buf, cur_x, cur_y, ' ');
      insert_char_to_buf(&f_buf, cur_x, cur_y, ' ');
      cur_x += 2;
      break;
    case KEY_BACKSPACE:
      if (cur_y <= 0 && cur_x <= 0) {
        return;
      }
      prev_line = get_line_length(cur_y - 1) - 1;
      remove_prev_char_from_buf(&f_buf, cur_x, cur_y);
      if (cur_x <= 0) {
        cur_y -= 1;
        cur_x = prev_line;
      } else {
        cur_x--;
      }
      break;
    case KEY_DC:
      if (cur_y >= f_buf.size - 1 && cur_x >= get_line_length(cur_y) - 1) {
        return;
      }
      remove_cur_char_from_buf(&f_buf, cur_x, cur_y);
      break;
    case 10:
      insert_new_line_char_to_buf(&f_buf, cur_x, cur_y);
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
        write_buf_to_file(&f_buf, f_path);
        memset(command, 0, sizeof(command));
      } else if (strcmp(command, "wq") == 0) {
        write_buf_to_file(&f_buf, f_path);
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

  cur_y_len = get_line_length(cur_y) - 2;
  adjust_cur_x();
  set_cur_scr();
}

// fix this: sometimes, it doesnt go to saved_x when supposed to...
void adjust_cur_x() {
  int line_len = get_line_length(cur_y) - 2;
  g_total_vlines = (line_len + m_max_width - 1) / m_max_width;
  g_cur_vline = (cur_x + m_max_width) / m_max_width;

  if (g_cur_vline <= g_total_vlines && g_total_vlines > 1) {
    cur_x = m_max_width * (g_cur_vline - 1) + saved_x;
  } else {
    cur_x = saved_x;
  }

  if (cur_x > line_len) {
    cur_x = line_len;
  } 

  if (line_len <= 0) {
    cur_x = 0;
  }
}

void set_cur_scr() {
  int total_vlines = 0;

  // Loop through each buffer line before current line
  for (int i = vport_start_y; i < cur_y; i++) {
      int line_len = strlen(f_buf.data[i]);
      int vlines_in_line = (line_len + m_max_width - 1) / m_max_width;
      total_vlines += vlines_in_line; // Add visual lines for each buffer line
  }

  int vlines = (cur_y_len + m_max_width - 1) / m_max_width;
  int cur_vline = (cur_x + m_max_width - 1) / m_max_width;

  cur_scr_x = cur_x % m_max_width;
  cur_scr_y = total_vlines + (cur_x/m_max_width);
}
