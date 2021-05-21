#include <bits/stdc++.h>

using namespace std;

mt19937 mt(random_device{}());
uniform_int_distribution<int> dist(1, 1000000);

#define T 100
#define N 5

inline int next_lexicographical_mask(int v) {
  int t = v | (v - 1);
  int w = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
  assert(__builtin_popcount(v) == N);
  return w;
}

void test() {
  int MAX = 1 << 10;
  int mask = (1 << 5) - 1;
  do {
    for (int i = 0; i < 10; i++) {
      putchar((mask & (1 << i)) ? '1' : '0');
    }
    putchar('\n');
    mask = next_lexicographical_mask(mask);
  } while (mask < MAX);
}

int main() {
  printf("%d\n", T);
  for (int t = 1; t <= T; t++) {
    printf("%d\n", N);
    for (int p = 0; p < 2; p++) {
      for (int n = 0; n < N; n++) {
        int a = dist(mt);
        int b = dist(mt);
        int c = dist(mt);
        if (n)
          putchar(' ');
        printf("%d %d %d", a, b, c);
      }
      printf("\n");
    }
  }
  return 0;
}
