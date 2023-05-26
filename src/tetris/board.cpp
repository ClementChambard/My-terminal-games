#include "board.h"
#include "../render.h"
#include "fallingPiece.h"
#include "pieceType.h"

board BOARD;

void board::draw() {
  ab_rect(2, 2, 3 + w, 3 + h, 90);
  if (effect) {
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        if (at(x, y))
          ab_pixel(x + 3, y + 3,
                   effectLines[y] ? (effect % 2 ? 0xFFFFFF : 0x000000)
                                  : TYPES[at(x, y)].col);
      }
    }
    effect--;
    if (effect == 0)
      contract();
  } else {
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        if (at(x, y))
          ab_pixel(x + 3, y + 3, TYPES[at(x, y)].col);
      }
    }
  }
}

void board::contract() {
  std::array<int, h> new_pos;
  int displacement = 0;
  for (int i = h - 1; i >= 0; i--) {
    if (effectLines[i]) {
      displacement++;
      effectLines[i] = false;
      new_pos[i] = -1;
      continue;
    }
    new_pos[i] = i + displacement;
  }
  auto copy = data;
  data = {};
  int oldy = -1;
  for (auto np : new_pos) {
    oldy++;
    if (np == -1)
      continue;
    for (int i = 0; i < w; i++)
      at(i, np) = copy[i + w * oldy];
  }
}

void board::check() {
  for (int i = 0; i < h; i++) {
    bool line = true;
    for (int j = 0; j < w; j++) {
      line = line && (at(j, i) != 0);
    }
    if (line) {
      effectLines[i] = true;
      effect = 6;
    }
  }
}

void board::put(const fallingPiece &p) {
  const int t = p.type;
  const auto &r = TYPES[t].rot(p.rotation);
  for (int i = 0; i < TYPES[t].size * TYPES[t].size; i++) {
    if (r.at(i)) {
      int xx = p.x + i % TYPES[t].size;
      int yy = p.y + i / TYPES[t].size;
      at(xx, yy) = t;
    }
  }
}
