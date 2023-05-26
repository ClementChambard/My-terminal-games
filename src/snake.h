#ifndef SNAKE_H
#define SNAKE_H

#include "game.h"

class snakeGame : public IGame {
public:
  void draw() override;
  void update(char c) override;
};

#endif // !SNAKE_H
