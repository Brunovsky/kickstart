#include <bits/stdc++.h>

int main() {
  int T = RAND_MAX / 2;
  for (int r = 0; r < 100; ++r) {
    for (int c = 0; c < 100; ++c) {
      if (rand() < T) {
        std::cout << 'A';
      } else {
        std::cout << 'B';
      }
    }
    std::cout << std::endl;
  }
  return 0;
}
