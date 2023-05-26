#include "tetris.h"
#include "../render.h"
#include "board.h"
#include "fallingPiece.h"
#include "pieceType.h"

void tetrisGame::update(char c) {

  if (BOARD.effect)
    return;

  if (c == 's') {
    if (checkCol(BOARD, PIECE.type, PIECE.rotation, PIECE.x, PIECE.y + 1))
      PIECE.y++;
    else
      PIECE.lock();
  }
  if (c == 'q')
    PIECE.move(-1);
  if (c == 'd')
    PIECE.move(1);
  if (c == 'z')
    PIECE.rotate();
  if (c == ' ') {
    PIECE.y += get_down_loc();
    PIECE.lock();
  }

  PIECE.tick_down();
}

void draw_next(int x = 19, int y = 3) {
  int t = show_next_type();
  ab_rect(x, y, x + 5, y + 5, 90);
  ab_cursor(x + 2, y - 1);
  ab_append("NEXT");
  x++;
  y++;
  if (TYPES[t].size < 4)
    y++;
  if (TYPES[t].size < 3)
    x++;
  draw_pieceType(t, 0, x, y, false, TYPES[t].size % 2);
}

void draw_score(int x = 19, int y = 10) {
  ab_cursor(x, y);
  ab_color(90);
  ab_appendf("SCORE: %06d", 0);
}

void tetrisGame::draw() {
  ab_append("\x1b[?25l");
  ab_clear();
  PIECE.draw(1, 1);
  PIECE.draw();
  BOARD.draw();
  draw_next();
  draw_score();

  ab_render();
}
