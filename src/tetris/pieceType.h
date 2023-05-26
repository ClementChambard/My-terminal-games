#ifndef PIECETYPE_INCLUDED_H
#define PIECETYPE_INCLUDED_H

#include "../render.h"
#include <vector>

enum T_piece { T_NONE = 0, T_I, T_T, T_S, T_Z, T_L, T_J, T_O, T_nb };

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

void draw_pieceType(int t, int r, int globX, int globY, bool white = false,
                    bool halfX = false);

extern pieceType TYPES[T_nb];

#endif // !PIECETYPE_INCLUDED_H
