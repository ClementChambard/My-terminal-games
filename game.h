#ifndef GAME_H
#define GAME_H

class IGame {
public:
  virtual void draw() = 0;
  virtual void update(char c) = 0;
};

#endif // !GAME_H
