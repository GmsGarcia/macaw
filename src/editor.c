#include "editor.h"
#include "buffer.h"
#include "input.h"
#include "render.h"
#include <curses.h>

enum MODE mode = NORMAL;
const char MODE_NAME[3][15] = {"NORMAL", "INSERT", "COMMAND"};

char *f_name;
DynamicBuffer f_buf;

char command[50];
char message[255];

int cur_y = 0, cur_x = 0, cur_line = 0;

void init_macaw(int argc, char *argv[]) {
  if (argc >= 2) {
    f_name = argv[1];
    read_file_to_buf(argv[1], &f_buf);
  } else {
    gen_new_buf(&f_buf);
  }
  run();
}

void run() {
  init_render();
  for (;;) {
    handle_input();
    render();
  }

  free_buf(&f_buf);
}

void set_mode(enum MODE m) {
  switch (m) {
  case NORMAL:
  case INSERT:
  case COMMAND:
    break;
  }
}
