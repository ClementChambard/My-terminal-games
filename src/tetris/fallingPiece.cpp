#include "fallingPiece.h"
#include "board.h"
#include "pieceType.h"
#include <array>

struct __xy {
  int x, y;
};

std::array<std::array<__xy, 5>, 8> jlstzSRSTable = {
    std::array<__xy, 5>{__xy{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    std::array<__xy, 5>{__xy{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    std::array<__xy, 5>{__xy{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    std::array<__xy, 5>{__xy{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    std::array<__xy, 5>{__xy{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    std::array<__xy, 5>{__xy{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    std::array<__xy, 5>{__xy{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    std::array<__xy, 5>{__xy{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}};

std::array<std::array<__xy, 5>, 8> iSRSTable = {
    std::array<__xy, 5>{__xy{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
    std::array<__xy, 5>{__xy{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
    std::array<__xy, 5>{__xy{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
    std::array<__xy, 5>{__xy{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
    std::array<__xy, 5>{__xy{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
    std::array<__xy, 5>{__xy{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
    std::array<__xy, 5>{__xy{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
    std::array<__xy, 5>{__xy{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}};

const std::array<__xy, 5> &getSRSLine(int type, int oldrot, int newrot) {
  int i = 0;
  if (oldrot == 1 && newrot == 0)
    i = 1;
  if (oldrot == 1 && newrot == 2)
    i = 2;
  if (oldrot == 2 && newrot == 1)
    i = 3;
  if (oldrot == 2 && newrot == 3)
    i = 4;
  if (oldrot == 3 && newrot == 2)
    i = 5;
  if (oldrot == 3 && newrot == 0)
    i = 6;
  if (oldrot == 0 && newrot == 3)
    i = 7;
  if (type == T_I)
    return iSRSTable[i];
  return jlstzSRSTable[i];
}

void fallingPiece::move(int dir) {
  if (checkCol(BOARD, type, rotation, x + dir, y))
    x += dir;
  maybe_reset_timer();
}

void fallingPiece::rotate(int dir) {
  if (type == T_O) {
    maybe_reset_timer();
    return;
  }

  int newrot = (rotation + dir) % 4;

  const std::array<__xy, 5> &srs = getSRSLine(type, rotation, newrot);

  for (const __xy p : srs) {
    if (checkCol(BOARD, type, newrot, x + p.x, y + p.y)) {
      x += p.x;
      y += p.y;
      rotation = newrot;
      break;
    }
  }
  maybe_reset_timer();
}

void fallingPiece::next_piece() {
  type = get_next_type();
  x = ((type == T_O) ? 4 : 3);
  y = -1;
  rotation = 0;
  reset_time = 0;
  grounded = false;
  time_of_last_tick = std::chrono::system_clock::now();
}

void fallingPiece::lock() {
  BOARD.put(*this);
  BOARD.check();
  next_piece();

  // lose condition here
  if (!checkCol(BOARD, type, rotation, x, y)) {
    exit(0);
  }
}

void fallingPiece::tick_down() {
  if (type == 0)
    next_piece();

  if (!checkCol(BOARD, type, rotation, x, y + 1)) {
    grounded = true;
    if (should_lock())
      lock();
    return;
  }
  grounded = false;
  if (!should_go_down())
    return;
  if (checkCol(BOARD, type, rotation, x, y + 1))
    y++;
  else {
    lock();
  }
}
void draw(bool white = 0, bool bottom = 0);

bool fallingPiece::should_go_down() {
  auto time_now = std::chrono::system_clock::now();
  long tick_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                           time_now - time_of_last_tick)
                           .count();
  if (tick_duration < tick_time)
    return false;
  time_of_last_tick = time_now;
  return true;
}

bool fallingPiece::should_lock() {
  auto time_now = std::chrono::system_clock::now();
  long tick_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                           time_now - time_of_last_tick)
                           .count();
  return tick_duration > lock_time;
}

void fallingPiece::maybe_reset_timer() {
  if (!grounded)
    return;
  if (reset_time++ < 15)
    time_of_last_tick = std::chrono::system_clock::now();
}

void fallingPiece::draw(bool white, bool bottom) {
  const auto &t = TYPES[type];
  const auto &r = t.rot(rotation);
  int b = (bottom ? get_down_loc() : 0);
  draw_pieceType(type, rotation, x + 3, y + 3 + b, white);
}

fallingPiece PIECE;

bool checkCol(board b, int type, int rot, int x, int y) {
  const auto &r = TYPES[type].rot(rot);
  for (int i = 0; i < TYPES[type].size * TYPES[type].size; i++) {
    if (r.at(i)) {
      int xx = x + i % TYPES[type].size;
      int yy = y + i / TYPES[type].size;
      if (xx < 0 || xx >= board::w)
        return false;
      if (yy < -20 || yy >= board::h)
        return false;
      if (yy < 0)
        continue;
      if (b.at(xx, yy))
        return false;
    }
  }
  return true;
}

int get_down_loc() {
  int i = 0;
  while (
      checkCol(BOARD, PIECE.type, PIECE.rotation, PIECE.x, PIECE.y + i + 1) &&
      i < 30) {
    i++;
  }
  return i;
}
