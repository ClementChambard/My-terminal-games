#include "random.h"

#include <algorithm>
#include <array>
#include <random>
#include <vector>

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
int show_next_type(int i) { return types.see_next(i); }
