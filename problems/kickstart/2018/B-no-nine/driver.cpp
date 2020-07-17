#include <bits/stdc++.h>

using namespace std;

// *****

using u64 = uint64_t;

#define MAXDIGITS 20

// magic[k][d][c] | k: number of digits | d: leading digit | c: congruence class
// d == 10 --> any leading digit
// c == 10 --> any congruence class
u64 magic[MAXDIGITS][11][11];

void setup() {
  memset(magic, 0, sizeof(magic));
  magic[0][0][0] = magic[0][10][0] = magic[0][0][10] = 1;
  for (int k = 1; k < MAXDIGITS; k++) {
    for (int d = 0; d < 9; d++) {
      for (int c = 0; c < 9; c++) {
        u64 tail = magic[k - 1][10][(c - d + 9) % 9];
        magic[k][d][c] += tail;
        magic[k][10][c] += tail;
        magic[k][d][10] += tail;
      }
    }
  }
}

void driver() {
  for (int k = 0; k < MAXDIGITS; k++) {
    printf("k = %d\n", k);
    for (int d = 0; d < 9; d++) {
      printf("magic[%d][%d]:", k, d);
      for (int c = 0; c < 9; c++) {
        printf(" %lu", magic[k][d][c]);
      }
      printf("\n");
    }
    printf("\n");
  }
}

int main() {
  setup();
  driver();
  return 0;
}
