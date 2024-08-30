#include <curses.h>
#include <stdlib.h>
#include <string.h>

#define MC_RED 16
#define MC_DARK_ORANGE 17
#define MC_ORANGE 18
#define MC_DARK_YELLOW 19
#define MC_YELLOW 20
#define MC_LIME 21
#define MC_GREEN 22
#define MC_CYAN 23
#define MC_BLUE 24
#define MC_DARK_BLUE 25
#define MC_PURPLE 26
#define MC_MAGENTA 27

#define MC_BLACK 28
#define MC_DARK_GRAY 29
#define MC_GRAY 30
#define MC_WHITE 31

void init_color_from_hex(int c, const char *hex) {
  int r, g, b;

  if (hex[0] == '#') {
    hex++;
  }

  /* if not 6 chars */
  if (strlen(hex) != 6) {
  }

  int r_dec = strtol(hex, NULL, 16) >> 16 & 0xFF;
  int g_dec = strtol(hex + 2, NULL, 16) >> 8 & 0xFF;
  int b_dec = strtol(hex + 4, NULL, 16) & 0xFF;

  // Scale the RGB values from 0-255 to 0-1000
  r = (r_dec * 1000) / 255;
  g = (g_dec * 1000) / 255;
  b = (b_dec * 1000) / 255;

  init_color(c, r, g, b);
}

void init_colors() {
  start_color();
  use_default_colors();

  init_color_from_hex(MC_RED, "#e23828");
  init_color_from_hex(MC_DARK_ORANGE, "#e25428");
  init_color_from_hex(MC_ORANGE, "#e26928");
  init_color_from_hex(MC_DARK_YELLOW, "#e29e28");
  init_color_from_hex(MC_YELLOW, "#e2d928");
  init_color_from_hex(MC_LIME, "#92e228");
  init_color_from_hex(MC_GREEN, "#28e23b");
  init_color_from_hex(MC_CYAN, "#28d3e2");
  init_color_from_hex(MC_BLUE, "#2892e2");
  init_color_from_hex(MC_DARK_BLUE, "#3828e2");
  init_color_from_hex(MC_PURPLE, "#8528e2");
  init_color_from_hex(MC_MAGENTA, "#e228df");

  init_color_from_hex(MC_BLACK, "#000000");
  init_color_from_hex(MC_DARK_GRAY, "#131313");
  init_color_from_hex(MC_GRAY, "#bbbbbb");
  init_color_from_hex(MC_WHITE, "#ffffff");

  init_pair(1, COLOR_BLACK, MC_RED);
  init_pair(2, COLOR_BLACK, MC_DARK_ORANGE);
  init_pair(3, COLOR_BLACK, MC_ORANGE);
  init_pair(4, COLOR_BLACK, MC_DARK_YELLOW);
  init_pair(5, COLOR_BLACK, MC_YELLOW);
  init_pair(6, COLOR_BLACK, MC_LIME);
  init_pair(7, COLOR_BLACK, MC_GREEN);
  init_pair(8, COLOR_BLACK, MC_CYAN);
  init_pair(9, COLOR_BLACK, MC_BLUE);
  init_pair(10, COLOR_BLACK, MC_DARK_BLUE);
  init_pair(11, COLOR_BLACK, MC_PURPLE);
  init_pair(12, COLOR_BLACK, MC_MAGENTA);

  init_pair(13, COLOR_BLACK, MC_BLACK);
  init_pair(14, COLOR_BLACK, MC_DARK_GRAY);
  init_pair(15, COLOR_BLACK, MC_DARK_GRAY);
  init_pair(16, COLOR_BLACK, MC_WHITE);
}
