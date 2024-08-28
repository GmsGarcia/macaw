#include <curses.h>

#ifndef RENDER_H
#define RENDER_H

extern WINDOW *m_win;
extern WINDOW *c_win;
extern WINDOW *b_win;

extern int m_height, m_width, m_y, m_x;
extern int b_height, b_width, b_y, b_x;

extern int m_max_height, m_max_width;

extern int colors;

void init_render();

void render();

void render_buf();

#endif
