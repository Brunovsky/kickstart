#include <bits/stdc++.h>

using namespace std;

using ui = uint64_t;

// *****

ui N;
vector<ui> A;

auto solve() {
  sort(A.begin(), A.end());

  ui count = 0;
  ui pos = 0;

  // deal with 0s with basic combinatorics
  while (pos < N && A[pos] == 0)
    ++pos;
  if (pos > 1) {
    count = (N - pos) * pos * (pos - 1) / 2;
    if (pos > 2) {
      count += pos * (pos - 1) * (pos - 2) / 6;
    }
  }

  // now deal with positives...
  for (ui i = pos; i + 2 < N; ++i) {
    for (ui j = i + 1; j + 1 < N; ++j) {
      ui P = A[i] * A[j];
      auto begin = A.begin() + j + 1, end = A.end();
      auto hi = upper_bound(begin, end, P);
      auto lo = lower_bound(begin, end, P);
      count += ui(hi - lo);
    }
  }

  return count;
}

// *****

void reparse_test() {
  cin >> N >> ws;
  A.assign(N, 0);
  for (ui i = 0; i < N; ++i)
    cin >> A[i] >> ws;
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
