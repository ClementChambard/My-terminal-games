#ifndef RENDER_H
#define RENDER_H

#include <string>

void start_raw_mode();
void ab_append(std::string const &);
void ab_render();

struct color {
  color(int hex) {
    b = hex & 0xff;
    hex >>= 8;
    g = hex & 0xff;
    hex >>= 8;
    r = hex & 0xff;
  }
  color(int r, int g, int b) : r(r), g(g), b(b) {}
  bool operator==(color const &other) {
    return r == other.r && g == other.g && b == other.b;
  }
  bool operator!=(color const &other) { return !(*this == other); }
  int r, g, b;
};

void ab_rect(int x1, int y1, int x2, int y2, int color_num = 37);
void ab_clear();
void ab_pixel_here(int color_num = 37);
void ab_pixel_here(color color_val);
void ab_pixel(int x1, int y1, int color_num = 37);
void ab_pixel(int x1, int y1, color color_val);
void ab_cursor(int x, int y);

#endif // !RENDER_H
