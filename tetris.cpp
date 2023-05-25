#include "tetris.h"
#include "render.h"
#include <algorithm>
#include <array>
#include <queue>
#include <random>

#include <chrono>

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

struct xy {
  int x, y;
};

std::array<std::array<xy, 5>, 8> jlstzSRSTable = {
    std::array<xy, 5>{xy{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    std::array<xy, 5>{xy{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    std::array<xy, 5>{xy{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}},
    std::array<xy, 5>{xy{0, 0}, {-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
    std::array<xy, 5>{xy{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}},
    std::array<xy, 5>{xy{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    std::array<xy, 5>{xy{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    std::array<xy, 5>{xy{0, 0}, {1, 0}, {1, 1}, {0, -2}, {1, -2}}};

std::array<std::array<xy, 5>, 8> iSRSTable = {
    std::array<xy, 5>{xy{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
    std::array<xy, 5>{xy{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
    std::array<xy, 5>{xy{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}},
    std::array<xy, 5>{xy{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
    std::array<xy, 5>{xy{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}},
    std::array<xy, 5>{xy{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}},
    std::array<xy, 5>{xy{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}},
    std::array<xy, 5>{xy{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}};

const std::array<xy, 5> &getSRSLine(int type, int oldrot, int newrot) {
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

board b;

bool checkCol(board b, int type, int rot, int addx, int addy);

struct fallingPiece {
  int x = 2, y = 0;
  int type = get_next_type();
  int rotation;
  int tick;

  long tick_time = 600;
  static constexpr long lock_time = 500;

  void move(int dir) {
    if (checkCol(b, type, rotation, x + dir, y))
      x += dir;
    maybe_reset_timer();
  }

  void rotate(int dir = 1) {
    if (type == T_O) {
      maybe_reset_timer();
      return;
    }

    int newrot = (rotation + dir) % 4;

    const std::array<xy, 5> &srs = getSRSLine(type, rotation, newrot);

    for (const xy p : srs) {
      if (checkCol(b, type, newrot, x + p.x, y + p.y)) {
        x += p.x;
        y += p.y;
        rotation = newrot;
        break;
      }
    }
    maybe_reset_timer();
  }

  void lock() {
    b.put(*this);
    b.check();
    x = 3;
    y = 0;
    rotation = 0;
    type = get_next_type();
    reset_time = 0;
    grounded = false;
    time_of_last_tick = std::chrono::system_clock::now();

    // lose condition here
    if (!checkCol(b, type, rotation, x, y)) {
      exit(0);
    }
  }

  void tick_down() {
    if (!checkCol(b, type, rotation, x, y + 1)) {
      grounded = true;
      if (should_lock())
        lock();
      return;
    }
    grounded = false;
    if (!should_go_down())
      return;
    if (checkCol(b, type, rotation, x, y + 1))
      y++;
    else {
      lock();
    }
  }
  void draw(bool white = 0, bool bottom = 0);

private:
  bool should_go_down() {
    auto time_now = std::chrono::system_clock::now();
    long tick_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                             time_now - time_of_last_tick)
                             .count();
    if (tick_duration < tick_time)
      return false;
    time_of_last_tick = time_now;
    return true;
  }

  bool should_lock() {
    auto time_now = std::chrono::system_clock::now();
    long tick_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                             time_now - time_of_last_tick)
                             .count();
    return tick_duration > lock_time;
  }

  void maybe_reset_timer() {
    if (!grounded)
      return;
    if (reset_time++ < 15)
      time_of_last_tick = std::chrono::system_clock::now();
  }

  bool grounded = false;
  int reset_time = 0;
  std::chrono::time_point<
      std::chrono::system_clock,
      std::chrono::duration<long, std::ratio<1, 1000000000>>>
      time_of_last_tick = std::chrono::system_clock::now();
};

fallingPiece fp;

typedef std::vector<bool> pieceRot;

struct pieceType {
  pieceType(pieceRot r0, pieceRot r1, pieceRot r2, pieceRot r3, int size,
            color col)
      : col(col), size(size), rot0(r0), rotR(r1), rot2(r2), rotL(r3) {}

  const pieceRot &rot(int i) const {
    return i == 0 ? rot0 : i == 1 ? rotR : i == 2 ? rot2 : rotL;
  }

  color col;
  int size;

private:
  const pieceRot rot0;
  const pieceRot rotR;
  const pieceRot rot2;
  const pieceRot rotL;
};

pieceType _piece_type_I = {{0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
                           {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                           {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
                           {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
                           4,
                           0x00FFFF};

pieceType _piece_type_T = {{0, 1, 0, 1, 1, 1, 0, 0, 0},
                           {0, 1, 0, 0, 1, 1, 0, 1, 0},
                           {0, 0, 0, 1, 1, 1, 0, 1, 0},
                           {0, 1, 0, 1, 1, 0, 0, 1, 0},
                           3,
                           0xAA00FF};

pieceType _piece_type_S = {{0, 1, 1, 1, 1, 0, 0, 0, 0},
                           {0, 1, 0, 0, 1, 1, 0, 0, 1},
                           {0, 0, 0, 0, 1, 1, 1, 1, 0},
                           {1, 0, 0, 1, 1, 0, 0, 1, 0},
                           3,
                           0x00FF00};

pieceType _piece_type_Z = {{1, 1, 0, 0, 1, 1, 0, 0, 0},
                           {0, 0, 1, 0, 1, 1, 0, 1, 0},
                           {0, 0, 0, 1, 1, 0, 0, 1, 1},
                           {0, 1, 0, 1, 1, 0, 1, 0, 0},
                           3,
                           0xFF0000};

pieceType _piece_type_L = {{0, 0, 1, 1, 1, 1, 0, 0, 0},
                           {0, 1, 0, 0, 1, 0, 0, 1, 1},
                           {0, 0, 0, 1, 1, 1, 1, 0, 0},
                           {1, 1, 0, 0, 1, 0, 0, 1, 0},
                           3,
                           0xFFAA00};

pieceType _piece_type_J = {{1, 0, 0, 1, 1, 1, 0, 0, 0},
                           {0, 1, 0, 0, 1, 0, 1, 1, 0},
                           {0, 0, 0, 1, 1, 1, 0, 0, 1},
                           {0, 1, 1, 0, 1, 0, 0, 1, 0},
                           3,
                           0x0000FF};

pieceType _piece_type_O = {
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, 2, 0xFFFF00};

pieceType TYPES[T_nb] = {
    {{}, {}, {}, {}, 0, 0}, _piece_type_I, _piece_type_T, _piece_type_S,
    _piece_type_Z,          _piece_type_L, _piece_type_J, _piece_type_O,
};

bool checkCol(board b, int type, int rot, int x, int y) {
  const auto &r = TYPES[type].rot(rot);
  for (int i = 0; i < TYPES[type].size * TYPES[type].size; i++) {
    if (r.at(i)) {
      int xx = x + i % TYPES[type].size;
      int yy = y + i / TYPES[type].size;
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

int get_down_loc() {
  int i = 0;
  while (checkCol(b, fp.type, fp.rotation, fp.x, fp.y + i + 1) && i < 30) {
    i++;
  }
  return i;
}

void fallingPiece::draw(bool white, bool bottom) {
  const auto &t = TYPES[type];
  const auto &r = t.rot(rotation);
  int b = (bottom ? get_down_loc() : 0);
  for (int i = 0; i < TYPES[type].size * TYPES[type].size; i++) {
    int posx = i % TYPES[type].size + x + 3;
    int posy = i / TYPES[type].size + y + 3 + b;
    if (r.at(i))
      ab_pixel(posx, posy, white ? 0x999999 : t.col);
  }
}

void tetrisGame::update(char c) {
  if (b.effect)
    return;
  if (c == 'q')
    fp.move(-1);
  if (c == 's') {
    if (checkCol(b, fp.type, fp.rotation, fp.x, fp.y + 1))
      fp.y++;
    else
      fp.lock();
  }
  if (c == 'd')
    fp.move(1);
  if (c == 'z')
    fp.rotate();
  if (c == ' ') {
    fp.y += get_down_loc();
    fp.lock();
  }
  if (c == 'p')
    fp.type = (fp.type + 1) % T_nb;
  fp.tick_down();
}

void tetrisGame::draw() {
  ab_append("\x1b[?25l");
  ab_clear();
  fp.draw(1, 1);
  fp.draw();
  b.draw();
  ab_render();
}
