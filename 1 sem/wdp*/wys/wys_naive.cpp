#include "wys.h"

int n, k, g;

bool smaller(int x) {
  int ile[2];
  ile[0] = ile[1] = 0;
  for (int i = 0; i < 2 * k + 1; ++i)
    ++ile[mniejszaNiz(x)];
  if (ile[0] > ile[1])
    return 0;
  else
    return 1;
}

void play(int lo, int hi) {
  if (lo == hi) {
    odpowiedz(lo);
    return;
  }
  int mid = (lo + hi + 1) / 2;
  if (smaller(mid))
    play(lo, mid - 1);
  else
    play(mid, hi);
}

int main() {
  dajParametry(n, k, g);
  while (g--) {
    play(1, n);
  }
}
