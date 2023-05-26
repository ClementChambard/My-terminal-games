#ifndef BOARD_INCLUDED_H
#define BOARD_INCLUDED_H

#include <array>

struct fallingPiece;

struct board {

  int &at(int x, int y) { return data[x + y * w]; }

  static constexpr int w = 10;
  static constexpr int h = 20;

  void put(fallingPiece const &);
  void check();
  void contract();

  void draw();

  int effect = 0;

private:
  std::array<bool, h> effectLines{};
  std::array<int, w * h> data{};
};

extern board BOARD;

#endif // !BOARD_INCLUDED_H
