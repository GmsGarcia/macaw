#ifndef EDITOR_H
#define EDITOR_H

enum MODE { NORMAL, INSERT, COMMAND };
extern const char MODE_NAME[3][15];

extern enum MODE mode;
extern char *f_name;
extern char *f_buffer;

extern char command[50];
extern char message[255];

extern int c_y, c_x;

void init_macaw(int argc, char *argv[]);

void run();

void set_mode(enum MODE mode);

#endif
