#include <bits/stdc++.h>

using namespace std;

using u32 = uint32_t;

random_device rng;
mt19937 mt(rng());

#define T 100
#define MIN_LENGTH 2
#define MAX_LENGTH 20
#define MIN_OFFSET 1
#define MAX_OFFSET N - MAX_LENGTH
#define LENGTH_STEP 0
#define OFFSET_STEP (15 * i)

uniform_int_distribution<int> Ndist(700000, 1000000);
uniform_int_distribution<int> Qdist(10000, 30000);
int main() {
  printf("%d\n", T);
  for (int t = 1; t <= T; t++) {
    uniform_int_distribution<u32> dist(0, 9);
    int N = Ndist(mt);
    int Q = Qdist(mt);
    printf("%d %d\n", N, Q);

    uniform_int_distribution<int> Length(MIN_LENGTH, MAX_LENGTH);
    uniform_int_distribution<int> Offset(MIN_OFFSET, MAX_OFFSET);
    for (int i = 1; i <= Q; i++) {
      int length = Length(mt) + LENGTH_STEP;
      int offset = Offset(mt) + OFFSET_STEP;
      int L = offset, R = offset + length - 1;
      printf("%d %d\n", L, R);
    }
  }
  return 0;
}
