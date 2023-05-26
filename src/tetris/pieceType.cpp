#include "pieceType.h"

static pieceType _piece_type_I = {
    {0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
    4,
    0x00FFFF};

static pieceType _piece_type_T = {{0, 1, 0, 1, 1, 1, 0, 0, 0},
                                  {0, 1, 0, 0, 1, 1, 0, 1, 0},
                                  {0, 0, 0, 1, 1, 1, 0, 1, 0},
                                  {0, 1, 0, 1, 1, 0, 0, 1, 0},
                                  3,
                                  0xAA00FF};

static pieceType _piece_type_S = {{0, 1, 1, 1, 1, 0, 0, 0, 0},
                                  {0, 1, 0, 0, 1, 1, 0, 0, 1},
                                  {0, 0, 0, 0, 1, 1, 1, 1, 0},
                                  {1, 0, 0, 1, 1, 0, 0, 1, 0},
                                  3,
                                  0x00FF00};

static pieceType _piece_type_Z = {{1, 1, 0, 0, 1, 1, 0, 0, 0},
                                  {0, 0, 1, 0, 1, 1, 0, 1, 0},
                                  {0, 0, 0, 1, 1, 0, 0, 1, 1},
                                  {0, 1, 0, 1, 1, 0, 1, 0, 0},
                                  3,
                                  0xFF0000};

static pieceType _piece_type_L = {{0, 0, 1, 1, 1, 1, 0, 0, 0},
                                  {0, 1, 0, 0, 1, 0, 0, 1, 1},
                                  {0, 0, 0, 1, 1, 1, 1, 0, 0},
                                  {1, 1, 0, 0, 1, 0, 0, 1, 0},
                                  3,
                                  0xFFAA00};

static pieceType _piece_type_J = {{1, 0, 0, 1, 1, 1, 0, 0, 0},
                                  {0, 1, 0, 0, 1, 0, 1, 1, 0},
                                  {0, 0, 0, 1, 1, 1, 0, 0, 1},
                                  {0, 1, 1, 0, 1, 0, 0, 1, 0},
                                  3,
                                  0x0000FF};

static pieceType _piece_type_O = {
    {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, {1, 1, 1, 1}, 2, 0xFFFF00};

pieceType TYPES[T_nb] = {
    {{}, {}, {}, {}, 0, 0}, _piece_type_I, _piece_type_T, _piece_type_S,
    _piece_type_Z,          _piece_type_L, _piece_type_J, _piece_type_O,
};

void draw_pieceType(int t, int r, int globX, int globY, bool white,
                    bool halfX) {
  for (int i = 0; i < TYPES[t].size * TYPES[t].size; i++) {
    int posx = i % TYPES[t].size + globX;
    int posy = i / TYPES[t].size + globY;
    if (TYPES[t].rot(r).at(i))
      ab_pixel(posx, posy, white ? 0x999999 : TYPES[t].col, halfX);
  }
}
