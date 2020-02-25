#include <bits/stdc++.h>

using namespace std;

// *****

uint N;            // number of pages
uint T;            // number of pages torn out
uint L;            // number of lazy readers
vector<uint> torn; // pages torn out
vector<uint> R;    // ith lazy reader reads multiples of R[i]

auto solve() {
  vector<bool> is_torn(N + 1, false);
  for (uint t : torn)
    is_torn[t] = true;

  vector<uint> torn_count(N + 1, 0);
  for (uint i = 1; i <= N; ++i)
    for (uint j = i; j <= N; j += i)
      torn_count[i] += is_torn[j] ? 0 : 1;

  uint64_t T = 0;
  for (uint r : R)
    T += torn_count[r];
  return T;
}

// *****

void reparse_test() {
  cin >> N >> T >> L;
  torn.resize(T);
  for (uint i = 0; i < T; ++i)
    cin >> torn[i];
  R.resize(L);
  for (uint i = 0; i < L; ++i)
    cin >> R[i];
  sort(R.begin(), R.end());
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
