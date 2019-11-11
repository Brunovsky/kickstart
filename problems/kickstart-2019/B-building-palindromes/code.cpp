#include <bits/stdc++.h>

using namespace std;

// *****

#define ASCII ('Z' - 'A' + 1)

using alph = array<int, ASCII>;

int N, Q;
string text;
vector<alph> prefix;
vector<int> L, R;

auto solve() {
  prefix.assign(N + 1, {});
  for (int n = 0; n < N; ++n) {
    prefix[n + 1] = prefix[n];
    ++prefix[n + 1][text[n] - 'A'];
  }
  int count = 0;
  for (int q = 0; q < Q; ++q) {
    const auto& right = prefix[R[q]];
    const auto& left = prefix[L[q] - 1];
    int odd = 0;
    for (int i = 0; i < ASCII; ++i) {
      if ((right[i] - left[i]) & 1) ++odd;
    }
    if (odd <= 1) ++count;
  }
  return count;
}

// *****

void reparse_test() {
  cin >> N >> Q >> ws;
  cin >> text >> ws;
  assert(text.size() == size_t(N));
  L.assign(Q, 0);
  R.assign(Q, 0);
  for (int i = 0; i < Q; ++i) cin >> L[i] >> R[i] >> ws;
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
