#include "editor.h"
#include "buffer.h"
#include "input.h"
#include "render.h"
#include <curses.h>
#include <string.h>

const int DEBUG = 0;

enum MODE mode = NORMAL;
const char MODE_NAME[3][15] = {"NORMAL", "INSERT", "COMMAND"};

char *f_path;
char *f_name;
DynamicBuffer f_buf;

char command[50];
char message[255];

int cur_y = 0, cur_x = 0;
int saved_x = 0;

int cur_vy = 0, cur_vx = 0;

int cur_y_len = 0;

void init_macaw(int argc, char *argv[]) {
  init_buf(&f_buf);

  if (argc >= 2) {
    f_path = argv[1];
    if (strrchr(f_path, '/') != NULL) {
      f_name = strrchr(f_path, '/') + 1;
    } else {
      f_name = f_path;
    }
    read_file_to_buf(&f_buf, f_path);
  } else {
    f_name = "new_file.txt";
    f_path = f_name;
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
