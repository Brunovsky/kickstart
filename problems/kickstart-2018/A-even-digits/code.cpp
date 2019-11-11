#include <bits/stdc++.h>

using namespace std;
using u64 = uint64_t;

// *****

u64 N;

u64 solve() {
  if (N < 10) return (N % 2) == 1;

  u64 m = 0, pow10 = 1;

  while (N > 10 * pow10) {
    pow10 *= 10;
    ++m;
  }

  u64 U = 0;
  u64 B = N;

  while (pow10 > 0) {
    u64 prevU = U;
    U = B / pow10;
    B = B % pow10;

    if (m == 0) return (U % 2) == 1;

    if (U % 2 == 1) {
      u64 up = pow10 - B;
      u64 down = B + 1 + (pow10 - 1) / 9UL;
      return U == 9 ? down : min(up, down);
    }

    pow10 /= 10;
    --m;
  }

  return 0UL;
}

// *****

void reparse_test() {
  cin >> N >> ws;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
