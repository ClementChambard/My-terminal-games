#include "game.h"
#include "render.h"
#include "snake.h"
#include "tetris.h"
#include <unistd.h>

IGame *game;

int main() {
  start_raw_mode();

  game = new tetrisGame;

  while (1) {
    char c = '\0';
    read(STDIN_FILENO, &c, 1);
    if (c == 'Q' - '@')
      break;
    game->update(c);
    game->draw();
  }

  return 0;
}
