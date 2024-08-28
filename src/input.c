#include "input.h"
#include "editor.h"
#include "render.h"
#include <ctype.h>
#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define ctrl(x) (x & 0x1F)

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
      if (c_x > 1) {
        c_x--;
      }
      break;
    case KEY_DOWN:
    case 'j':
      if (c_y < getmaxy(m_win) - 2) {
        c_y++;
      }
      break;
    case KEY_UP:
    case 'k':
      if (c_y > 1) {
        c_y--;
      }
      break;
    case KEY_RIGHT:
    case 'l':
      if (c_x < getmaxx(m_win) - 2) {
        c_x++;
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

  wmove(m_win, c_y, c_x);
}
