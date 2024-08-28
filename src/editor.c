#include "editor.h"
#include "buffer.h"
#include "input.h"
#include "render.h"

enum MODE mode = NORMAL;
const char MODE_NAME[3][15] = {"NORMAL", "INSERT", "COMMAND"};

char *f_name;
char *f_buffer = 0;

char command[50];
char message[255];

int c_y = 1, c_x = 1;

void init_macaw(int argc, char *argv[]) {
  if (argc >= 2) {
    load_buf(argv[1]);
  } else {
    gen_new_buf();
  }
  run();
}

void run() {
  init_render();
  for (;;) {
    handle_input();
    render();
  }
}

void set_mode(enum MODE m) {
  switch (m) {
  case NORMAL:
  case INSERT:
  case COMMAND:
    break;
  }
}
