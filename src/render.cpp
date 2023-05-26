#include "render.h"
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <vector>

struct appendBuffer {
  void append(std::string const &s) {
    for (char c : s)
      ab.push_back(c);
  }
  void appendf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    static char buf[100];
    vsprintf(buf, fmt, args);
    append(buf);
  }
  void render() {
    write(STDIN_FILENO, ab.data(), ab.size());
    ab.clear();
  }
  std::vector<char> ab;
};

appendBuffer ab;
int lastCol = 37;
color lastCol_val = 0xFFFFFF;

void ab_pixel_here(color color_val) {
  ab_color(color_val);
  ab.append("\u2588\u2588");
}

void ab_pixel(int x1, int y1, color color_val, bool halfX) {
  ab_cursor(x1, y1, halfX);
  ab_pixel_here(color_val);
}

void ab_append(std::string const &s) { ab.append(s); }
void ab_appendf(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  static char buf[100];
  vsprintf(buf, fmt, args);
  ab.append(buf);
}

void ab_color(int color_num) {
  if (lastCol != color_num) {
    lastCol = color_num;
    ab.appendf("\x1b[%dm", color_num);
  }
}

void ab_color(color color_val) {
  if (lastCol != 38 || lastCol_val != color_val) {
    lastCol = 38;
    lastCol_val = color_val;
    ab.appendf("\x1b[38;2;%d;%d;%dm", color_val.r, color_val.g, color_val.b);
  }
}

void ab_render() { ab.render(); }

void ab_clear() { ab.append("\x1b[H\x1b[J"); }

void ab_pixel_here(int col) {
  ab_color(col);
  ab.append("\u2588\u2588");
}

void ab_pixel(int x1, int y1, int col, bool halfX) {
  ab_cursor(x1, y1, halfX);
  ab_pixel_here(col);
}

void ab_cursor(int x, int y, bool halfX) {
  ab.appendf("\x1b[%d;%dH", y, x * 2 + halfX);
}

void ab_rect(int x1, int y1, int x2, int y2, int color_num) {
  for (int i = y1; i <= y2; i++) {
    ab_pixel(x1, i, color_num);
    if (i == y1 || i == y2)
      for (int j = x1 + 1; j <= x2; j++)
        ab_pixel_here(color_num);
    else
      ab_pixel(x2, i, color_num);
  }
}

struct termios orig_termios;
void die(const char *s) {
  perror(s);
  exit(1);
}

void disableRawMode() {
  printf("\x1b[?25h\x1b[H\x1b[J");
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    die("tcsetattr");
}

void enableRawMode() {
  if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    die("tcgetattr");
  atexit(disableRawMode);

  struct termios raw = orig_termios;

  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  raw.c_oflag &= ~(OPOST);
  raw.c_cflag |= (CS8);
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  raw.c_cc[VMIN] = 0;
  raw.c_cc[VTIME] = 1;

  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    die("tcsetattr");
}

void start_raw_mode() { enableRawMode(); }
