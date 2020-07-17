#include <bits/stdc++.h>

using namespace std;
using u64 = uint64_t;

// *****

u64 compute(u64 N) {
  if (N <= 2) {
    return N & 1;
  }

  u64 x200 = 20;
  u64 x888 = 8;

  while (N > x200) {
    x200 = 10 * x200;
    x888 = 10 * x888 + 8;
  }
  if (x888 <= N && N <= x200) {
    return min(N - x888, x200 - N);
  }
  x200 /= 10;
  assert(x200 <= N && N < x888);

  // while the leading digit of N is even
  u64 leading_digit = N / (x200 / 2);
  while (x200 > 2 && (leading_digit & 1) == 0) {
    N = N - leading_digit * x200 / 2;
    x200 /= 10, x888 /= 10;
    leading_digit = N / (x200 / 2);
  }
  if (N < 10) {
    return N & 1;
  }
  u64 lower = (leading_digit / 2) * x200 + (x888 / 10);
  if (leading_digit == 9) {
    return N - lower;
  }
  u64 higher = (1 + leading_digit / 2) * x200;
  return min(N - lower, higher - N);
}

u64 solve() {
  u64 N;
  cin >> N >> ws;
  return compute(N);
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
