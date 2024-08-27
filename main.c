#include <ctype.h>
#include <curses.h>
#include <ncurses.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ctrl(x) (x & 0x1F)

enum MODE { NORMAL, INSERT, COMMAND };
char MODE_NAME[3][15] = {"NORMAL", "INSERT", "COMMAND"};

enum MODE mode = NORMAL;
char *buffer = 0;
char command[50];
char message[255];

char *filename;

WINDOW *m_win;
WINDOW *b_win;

int m_height, m_width, m_y, m_x;
int b_height, b_width, b_y, b_x;

int c_y = 1, c_x = 1;

bool needs_refresh = true;

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

void render() {
  curs_set(0);
  werase(m_win);
  werase(b_win);

  wborder(m_win, 0, 0, 0, 0, 0, 0, 0, 0);
  wborder(b_win, 0, 0, 0, 0, 0, 0, 0, 0);

  if (mode == COMMAND) {
    mvwprintw(b_win, 1, 2, "%s :%s", MODE_NAME[mode], command);
  } else if (strlen(message) > 0) {
    mvwprintw(b_win, 1, 2, "%s - %s", MODE_NAME[mode], message);
  } else {
    mvwprintw(b_win, 1, 2, "%s", MODE_NAME[mode]);
  }
  // Print buffer content with proper handling of borders
  int max_width = getmaxx(m_win) - 2; // Subtract 2 for borders
  int max_height = getmaxy(m_win) - 2;
  int y = 1, x = 1;
  for (char *p = buffer; *p && y <= max_height; p++) {
    if (*p == '\n' || x > max_width) {
      y++;
      x = 1;
      if (*p != '\n') {
        mvwprintw(m_win, y, x, "%c", *p);
      }
    } else {
      mvwprintw(m_win, y, x++, "%c", *p);
    }
  }

  char c_pos[20];
  sprintf(c_pos, "%d:%d", c_y, c_x);

  int c_pos_size = getmaxx(b_win) - strlen(c_pos) - 2;
  mvwprintw(b_win, 1, c_pos_size, "%s", c_pos);

  wrefresh(m_win);
  wrefresh(b_win);

  wmove(m_win, c_y, c_x);
  curs_set(2);
  needs_refresh = false;
}

int main(int argc, char *argv[]) {
  initscr();

  m_height = LINES - 3, m_width = COLS, m_y = 0, m_x = 0;
  b_height = 3, b_width = COLS, b_y = LINES - 3, b_x = 0;

  noecho();
  curs_set(2);

  m_win = newwin(m_height, m_width, m_y, m_x);
  b_win = newwin(b_height, b_width, b_y, b_x);

  wborder(m_win, 0, 0, 0, 0, 0, 0, 0, 0);
  wborder(b_win, 0, 0, 0, 0, 0, 0, 0, 0);

  wmove(m_win, c_y, c_x);

  raw();
  keypad(stdscr, TRUE);
  keypad(m_win, TRUE);

  if (argc >= 2) {
    filename = argv[1];
    FILE *file = fopen(filename, "r");

    if (file) {
      fseek(file, 0, SEEK_END);
      long length = ftell(file);
      fseek(file, 0, SEEK_SET);

      if (length > 0) {
        buffer = malloc(length);
        if (buffer) {
          fread(buffer, 1, length, file);
        }
      } else {
        buffer = malloc(1);
        if (buffer) {
          buffer[0] = '\0';
        }
      }
      fclose(file);

      if (buffer) {
        render();
        for (;;) {
          handle_input();
          render();
        }
      } else {
        endwin();
        printf("No buffer.\n");
      }

    } else {
      endwin();
      printf("%s doesn't exist.\n", filename);
    }
  } else {
    endwin();
    printf("No file specified.\n");
  }

  return 0;
}
