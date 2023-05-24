#ifndef RENDER_H
#define RENDER_H

#include <string>

void start_raw_mode();
void ab_append(std::string const &);
void ab_render();

void ab_rect(int x1, int y1, int x2, int y2, int color_num = 37);
void ab_clear();
void ab_pixel_here(int color_num = 37);
void ab_pixel(int x1, int y1, int color_num = 37);
void ab_cursor(int x, int y);

#endif // !RENDER_H
