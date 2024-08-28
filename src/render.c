#include "render.h"
#include "editor.h"
#include <curses.h>
#include <string.h>

WINDOW *m_win;
WINDOW *b_win;

int m_height, m_width, m_y, m_x;
int b_height, b_width, b_y, b_x;

int m_max_height, m_max_width;

void init_render() {
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

  m_max_width = getmaxx(m_win) - 2;
  m_max_height = getmaxy(m_win) - 2;

  render();
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
  int y = 1, x = 1;
  for (char *p = f_buffer; *p && y <= m_max_height; p++) {
    if (*p == '\n' || x > m_max_width) {
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
}
