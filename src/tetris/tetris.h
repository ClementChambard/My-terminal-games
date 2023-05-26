#ifndef TETRIS_H
#define TETRIS_H

#include "../game.h"

class tetrisGame : public IGame {
  void update(char c) override;
  void draw() override;
};

#endif // !TETRIS_H
