#ifndef FALLINGPIECE_INCLUDED_H
#define FALLINGPIECE_INCLUDED_H

#include "random.h"
#include <chrono>

struct fallingPiece {
  int x = 0, y = 0;
  int type = 0;
  int rotation = 0;

  long tick_time = 600;
  static constexpr long lock_time = 500;

  void move(int dir);
  void rotate(int dir = 1);
  void lock();
  void tick_down();
  void draw(bool white = 0, bool bottom = 0);

private:
  bool should_go_down();
  bool should_lock();
  void maybe_reset_timer();
  void next_piece();

  bool grounded = false;
  int reset_time = 0;
  std::chrono::time_point<
      std::chrono::system_clock,
      std::chrono::duration<long, std::ratio<1, 1000000000>>>
      time_of_last_tick = std::chrono::system_clock::now();
};

struct board;
bool checkCol(board b, int type, int rot, int x, int y);
int get_down_loc();

extern fallingPiece PIECE;

#endif // !FALLINGPIECE_INCLUDED_H
