#include "render.h"
#include "buffer.h"
#include "color.h"
#include "editor.h"
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// main window
WINDOW *m_win;
// counter window - line number
WINDOW *c_win;
// bottom bar
WINDOW *b_win;
// terminal bar - messages container
WINDOW *t_win;

int m_height, m_width, m_y, m_x;
int c_height, c_width, c_y, c_x;

int b_height, b_width, b_y, b_x;
int t_height, t_width, t_y, t_x;

int m_max_height, m_max_width;

int vport_start_y, vport_height;

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
  b_height = 1, b_width = COLS, b_y = LINES - 2, b_x = 0;
  t_height = 1, t_width = COLS, t_y = LINES - 1, t_x = 0;

  noecho();

  m_win = newwin(m_height, m_width, m_y, m_x);
  c_win = newwin(c_height, c_width, c_y, c_x);
  b_win = newwin(b_height, b_width, b_y, b_x);
  t_win = newwin(t_height, t_width, t_y, t_x);

  wmove(m_win, cur_scr_y, cur_x);

  raw();
  keypad(stdscr, TRUE);
  keypad(m_win, TRUE);

  m_max_width = getmaxx(m_win) - 1;
  m_max_height = getmaxy(m_win);

  vport_start_y = cur_y;
  vport_height = m_height;

  curs_set(2);

  render();
}

void render() {
  curs_set(0);
  werase(m_win);
  werase(c_win);
  werase(b_win);
  werase(t_win);

  render_buf();
  render_info();

  wrefresh(m_win);
  wrefresh(c_win);
  wrefresh(b_win);
  wrefresh(t_win);

  wmove(m_win, cur_scr_y, cur_x);

  if (mode != COMMAND) {
    curs_set(2);
  }
}

void render_info() {
  char cur_pos[30];
  sprintf(cur_pos, " %d:%d ", cur_y + 1, cur_x + 1);
  int cur_pos_size = getmaxx(b_win) - strlen(cur_pos) - 2;

  wbkgd(b_win, COLOR_PAIR(14));

  int color_pair = 7;

  switch (mode) {
    case NORMAL:
      color_pair = 7;
      break;
    case INSERT:
      color_pair = 9;
      break;
    case COMMAND:
      color_pair = 11;
      break;
  }

  wattron(b_win, COLOR_PAIR(color_pair));
  if (DEBUG) {
    mvwprintw(b_win, 0, 0, " %s > %s - vport_heigth: %d - vport_start_y: %d - cur_scr_y: %d", MODE_NAME[mode], f_name, vport_height, vport_start_y, cur_scr_y+1);
  } else {
    mvwprintw(b_win, 0, 0, " %s > %s ", MODE_NAME[mode], f_name);
  }

  mvwprintw(b_win, 0, cur_pos_size, " %s ", cur_pos);
  wattroff(b_win, COLOR_PAIR(color_pair));

  if (mode == COMMAND) {
    mvwprintw(t_win, 0, 0, ":%s ", command);
  } else {
    if (strlen(message) > 0) {
      mvwprintw(t_win, 0, 0, "%s ", message);
    }
  }
}

void render_buf() {
  int y = 0, x = 0; // Start rendering from (1, 1) to avoid border overlap
  int max_y = m_max_height;
  int max_x = m_max_width;

  // Iterate through the buffer data and render each character
  mvwprintw(c_win, y, 0, "%d", y + 1);

  for (size_t i = vport_start_y; i < vport_start_y + vport_height && i < f_buf.size; i++) {
    char *line = f_buf.data[i];
    size_t line_length = strlen(line);
    size_t current_line_length = 0;
    size_t x = 0;

    mvwprintw(c_win, y, 0, "%d", (int)i + 1);
    for (size_t j = 0; j < line_length; j++) {
      char ch = line[j];

      // Handle line wrapping
      if (current_line_length >= max_x) {
        y++;
        x = 0;
        current_line_length = 0;
      }

      // Render the character
      mvwaddch(m_win, y, x, ch);
      x++;
      current_line_length++;

      // Stop rendering if we reach the bottom of the window
      if (y >= max_y) {
        break;
      }
    }

    // Move to the next line in the terminal window
    y++;

    // Stop rendering if we reach the bottom of the window
    if (y >= max_y) {
      break;
    }
  }
  // Move the cursor back to the current editing position
  wmove(m_win, cur_scr_y, cur_x);
}
