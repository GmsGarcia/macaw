#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

extern const int DEBUG;

enum MODE { NORMAL, INSERT, COMMAND };
extern const char MODE_NAME[3][15];

extern enum MODE mode;
extern char *f_name;
extern char *f_path;
extern DynamicBuffer f_buf;

extern char command[50];
extern char message[255];

extern int cur_y, cur_x;

void init_macaw(int argc, char *argv[]);

void run();

void set_mode(enum MODE mode);

#endif
