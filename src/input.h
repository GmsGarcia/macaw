#ifndef INPUT_H
#define INPUT_H()

extern int g_total_vlines, g_cur_vline;

void handle_input();

void adjust_cur_x(int force_len);

void set_cur_scr();

#endif
