#include "snake.h"
#include "render.h"
#include <sstream>
#include <vector>

struct pos {
  int x;
  int y;
};

std::vector<pos> snake = {{10, 10}, {10, 9}, {10, 8}, {10, 7}, {10, 6},
                          {10, 6},  {10, 6}, {10, 6}, {10, 6}, {10, 6},
                          {10, 6},  {10, 6}, {10, 6}, {10, 6}, {10, 6}};

void snakeGame::update(char c) {
  static int dir = 0;
  if (c == 'z')
    dir = 3;
  if (c == 'q')
    dir = 1;
  if (c == 's')
    dir = 2;
  if (c == 'd')
    dir = 0;
  if (c == 'a')
    snake.push_back(snake.back());
  if (c == 'e')
    snake.pop_back();

  for (int i = snake.size() - 2; i >= 0; i--)
    snake[i + 1] = snake[i];

  switch (dir) {
  case 0:
    snake[0].x++;
    if (snake[0].x > 30)
      snake[0].x = 1;
    break;
  case 1:
    snake[0].x--;
    if (snake[0].x < 1)
      snake[0].x = 30;
    break;
  case 2:
    snake[0].y++;
    if (snake[0].y > 30)
      snake[0].y = 1;
    break;
  case 3:
    snake[0].y--;
    if (snake[0].y < 1)
      snake[0].y = 30;
    break;
  }
}

void snakeGame::draw() {
  ab_append("\x1b[?25l");
  ab_clear();
  ab_rect(5, 5, 36, 36);

  int i = 0;
  for (auto p : snake) {
    int col = i++ % 2 ? 32 : i == 1 ? 31 : 34;
    ab_pixel(p.x + 5, p.y + 5, col);
  }

  ab_render();
}
