#include "tetris.h"
#include "render.h"
#include <algorithm>
#include <array>
#include <queue>
#include <random>

enum T_piece { T_NONE = 0, T_I, T_T, T_S, T_Z, T_L, T_J, T_O, T_nb };

struct fallingPiece;

struct next_queue {

  int next() {
    if (ptr > 13) {
      first_alloc();
    }
    if (ptr > 6) {
      next_alloc();
    }
    return data[ptr++];
  }

  int see_next(int i) {
    if (ptr > 13) {
      first_alloc();
    }
    if (ptr > 6) {
      next_alloc();
    }
    return data[ptr + i];
  }

private:
  void first_alloc() {
    ptr = 0;
    more();
    shift();
    more();
  }
  void next_alloc() {
    ptr = 0;
    shift();
    more();
  }
  void shift() {
    for (int i = 0; i < 7; i++)
      data[i] = data[i + 7];
  }
  void more() {
    std::vector<int> d = {1, 2, 3, 4, 5, 6, 7};
    static std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(d.begin(), d.end(), g);
    for (int i = 0; i < 7; i++)
      data[i] = d[i];
  }
  int ptr = 16;
  std::array<int, 14> data{};
} types;

int get_next_type() { return types.next(); }

int show_next_type(int i = 0) { return types.see_next(i); }

struct board {

  int &at(int x, int y) { return data[x + y * w]; }

  static constexpr int w = 11;
  static constexpr int h = 19;

  void put(fallingPiece const &);
  void check();
  void contract();

  void draw();

  int effect = 0;

private:
  std::array<bool, h> effectLines{};
  std::array<int, w * h> data{};
};

board b;

bool checkCol(board b, int type, int rot, int addx, int addy);

struct fallingPiece {
  int x, y;
  int type;
  int rotation;
  int tick;
  void rotate() {
    int newrot = (rotation + 1) % 4;
    if (checkCol(b, type, newrot, x, y))
      rotation = newrot;
  }
  void tick_down() {
    if (type == 0)
      type = get_next_type();
    if (checkCol(b, type, rotation, x, y + 1))
      y++;
    else {
      b.put(*this);
      b.check();
      x = 3;
      y = 0;
      type = types.next();
      rotation = 0;
    }
  }
  void draw();
};

fallingPiece fp;

struct pieceRot {
  pieceRot(bool a00, bool a10, bool a20, bool a30, bool a01, bool a11, bool a21,
           bool a31, bool a02, bool a12, bool a22, bool a32, bool a03, bool a13,
           bool a23, bool a33)
      : table({a00, a10, a20, a30, a01, a11, a21, a31, a02, a12, a22, a32, a03,
               a13, a23, a33}) {}
  int at(int x, int y) const { return table[x + y * 4]; }
  int at(int i) const { return table[i]; }

private:
  std::array<bool, 16> table;
};

struct pieceType {
  pieceType(pieceRot r0, pieceRot r1, pieceRot r2, pieceRot r3, int col)
      : col(col), rotations({r0, r1, r2, r3}) {}

  const pieceRot &rot(int i) const { return rotations[i]; }

  int col;

private:
  const std::array<pieceRot, 4> rotations;
};

pieceType _piece_type_I = {{0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           96};

pieceType _piece_type_T = {{0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                           35};

pieceType _piece_type_S = {{0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0},
                           32};

pieceType _piece_type_Z = {{0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                           31};

pieceType _piece_type_L = {{0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0},
                           {0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0},
                           33};

pieceType _piece_type_J = {{0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                           {1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0},
                           34};

pieceType _piece_type_O = {{0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                           {0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0},
                           93};

pieceType TYPES[T_nb] = {
    {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
     0},
    _piece_type_I,
    _piece_type_T,
    _piece_type_S,
    _piece_type_Z,
    _piece_type_L,
    _piece_type_J,
    _piece_type_O,
};

bool checkCol(board b, int type, int rot, int x, int y) {
  const auto &r = TYPES[type].rot(rot);
  for (int i = 0; i < 16; i++) {
    if (r.at(i)) {
      int xx = x + i % 4;
      int yy = y + i / 4;
      if (xx < 0 || xx >= board::w)
        return false;
      if (yy < 0 || yy >= board::h)
        return false;
      if (b.at(xx, yy))
        return false;
    }
  }
  return true;
}

void board::draw() {
  ab_rect(2, 2, 3 + w, 3 + h, 90);
  if (effect) {
    for (int y = 0; y < h; y++) {
      for (int x = 0; x < w; x++) {
        if (at(x, y))
          ab_pixel(x + 3, y + 3,
                   effectLines[y] ? (effect % 2 ? 37 : 30)
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
      effect = 8;
    }
  }
}

void board::put(const fallingPiece &p) {
  const int t = p.type;
  const auto &r = TYPES[t].rot(p.rotation);
  for (int i = 0; i < 16; i++) {
    if (r.at(i)) {
      int xx = p.x + i % 4;
      int yy = p.y + i / 4;
      at(xx, yy) = t;
    }
  }
}

void fallingPiece::draw() {
  const auto &t = TYPES[type];
  const auto &r = t.rot(rotation);
  for (int i = 0; i < 16; i++) {
    int posx = i % 4 + x + 3;
    int posy = i / 4 + y + 3;
    if (r.at(i))
      ab_pixel(posx, posy, t.col);
  }
}

void tetrisGame::update(char c) {
  if (b.effect)
    return;
  if (c == 'q')
    if (checkCol(b, fp.type, fp.rotation, fp.x - 1, fp.y))
      fp.x--;
  if (c == 's')
    fp.tick_down();
  if (c == 'd')
    if (checkCol(b, fp.type, fp.rotation, fp.x + 1, fp.y))
      fp.x++;
  if (c == 'z')
    fp.rotate();
  if (c == 'p')
    fp.type = (fp.type + 1) % T_nb;
}

void tetrisGame::draw() {
  ab_append("\x1b[?25l");
  ab_clear();
  fp.draw();
  b.draw();
  ab_render();
}
