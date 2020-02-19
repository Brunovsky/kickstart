#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;
using u64 = uint64_t;

// *****

u32 N, K;
vector<u64> V;

auto solve() {
  u64 sum = 0;
  for (u64 v : V) sum += v;

  if (K == 0) {
    double mean = double(sum) / double(N);
    return mean;
  }
}

// *****

void reparse_test() {
  cin >> N >> K;
  V.assign(N, 0);
  for (u32 i = 0; i < N; ++i) cin >> V[i];
  sort(V.begin(), V.end());
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
