#include "input.h"
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
      if (cur_y < m_max_height) {
        cur_y++;
      }
      break;
    case KEY_UP:
    case 'k':
      if (cur_y > 0) {
        cur_y--;
      }
      break;
    case KEY_RIGHT:
    case 'l':
      if (cur_x < m_max_width) {
        cur_x++;
      }
      break;
    case 'i':
      mode = INSERT;
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
    if (c == 27 || c == ctrl('c')) {
      mode = NORMAL;
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
        sprintf(message, "TODO: implement save buffer");
        memset(command, 0, sizeof(command));
      } else if (strcmp(command, "wq") == 0) {
        endwin();
        printf("TODO: wq not saving to buffer yet.\n");
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
