#include "render.h"
#include "editor.h"
#include "utils.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

WINDOW *m_win;
WINDOW *c_win;
WINDOW *b_win;

int m_height, m_width, m_y, m_x;
int c_height, c_width, c_y, c_x;
int b_height, b_width, b_y, b_x;

int m_max_height, m_max_width;

int colors = 0;

void init_render() {
  initscr();

  if (has_colors() == FALSE) {
    endwin();
    printf("Your terminal doesn't support colors\n");
    exit(1);
  }

  init_colors();

  m_height = LINES - 2, m_width = COLS - 4, m_y = 0, m_x = 4;
  c_height = LINES - 2, c_width = 4, c_y = 0, c_x = 0;
  b_height = 2, b_width = COLS, b_y = LINES - 2, b_x = 0;

  noecho();
  curs_set(2);

  m_win = newwin(m_height, m_width, m_y, m_x);
  c_win = newwin(c_height, c_width, c_y, c_x);
  b_win = newwin(b_height, b_width, b_y, b_x);

  wmove(m_win, cur_y, cur_x);

  raw();
  keypad(stdscr, TRUE);
  keypad(m_win, TRUE);

  m_max_width = getmaxx(m_win) - 1;
  m_max_height = getmaxy(m_win) - 1;

  render();
}

void render() {
  curs_set(0);
  werase(m_win);
  werase(c_win);
  werase(b_win);

  render_buf();

  char cur_pos[30];
  sprintf(cur_pos, " %d:%d ", cur_y + 1, cur_x + 1);
  int cur_pos_size = getmaxx(b_win) - strlen(cur_pos) - 2;

  wbkgd(b_win, COLOR_PAIR(14));
  // TODO: somehow remove this redundancy
  switch (mode) {
  case NORMAL:
    wattron(b_win, COLOR_PAIR(7));
    if (strlen(message) > 0) {
      mvwprintw(b_win, 0, 0, " %s > %s - %s ", MODE_NAME[mode], f_name,
                message);
    } else {
      mvwprintw(b_win, 0, 0, " %s > %s ", MODE_NAME[mode], f_name);
    }
    mvwprintw(b_win, 0, cur_pos_size, " %s ", cur_pos);
    wattroff(b_win, COLOR_PAIR(7));
    break;
  case INSERT:
    wattron(b_win, COLOR_PAIR(9));
    if (strlen(message) > 0) {
      mvwprintw(b_win, 0, 0, " %s > %s - %s ", MODE_NAME[mode], f_name,
                message);
    } else {
      mvwprintw(b_win, 0, 0, " %s > %s ", MODE_NAME[mode], f_name);
    }
    mvwprintw(b_win, 0, cur_pos_size, " %s ", cur_pos);
    wattroff(b_win, COLOR_PAIR(9));
    break;
  case COMMAND:
    wattron(b_win, COLOR_PAIR(11));
    mvwprintw(b_win, 0, 0, " %s > %s :%s ", MODE_NAME[mode], f_name, command);
    mvwprintw(b_win, 0, cur_pos_size, " %s ", cur_pos);
    wattroff(b_win, COLOR_PAIR(11));
    break;
  }

  wrefresh(m_win);
  wrefresh(c_win);
  wrefresh(b_win);

  wmove(m_win, cur_y, cur_x);
  curs_set(2);
}

void render_buf() {
  int y = 0, x = 0; // Start rendering from (1, 1) to avoid border overlap
  int max_y = m_max_height;
  int max_x = m_max_width;
  int current_line_length = 0;

  // Iterate through the buffer data and render each character
  mvwprintw(c_win, y, 0, "%d", y + 1);
  for (size_t i = 0; i < f_buf.length; i++) {
    char ch = f_buf.data[i];

    // Handle newlines and reset position
    if (ch == '\n' || current_line_length >= max_x) {
      mvwprintw(c_win, y, 0, "%d", y + 1);
      y++;
      x = 0;
      current_line_length = 0;
    } else {
      mvwaddch(m_win, y, x, ch);
      x++;
      current_line_length++;
    }

    // Stop rendering if we reach the bottom of the window
    if (y >= max_y) {
      break;
    }
  }

  // Move the cursor back to the current editing position
  wmove(m_win, cur_y, cur_x);
}
