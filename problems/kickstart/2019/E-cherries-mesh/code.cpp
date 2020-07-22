#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;
using i8 = int8_t;

// *****

u32 N, M;
vector<vector<u32>> adj;
vector<i8> tag;

void dfs(u32 i) {
  tag[i] = 1;
  for (u32 j : adj[i]) {
    if (!tag[j])
      dfs(j);
  }
}

auto solve() {
  cin >> N >> M >> ws;
  adj.assign(N, {});
  tag.assign(N, 0);
  for (u32 m = 0; m < M; ++m) {
    u32 Ci, Di;
    cin >> Ci >> Di >> ws;
    --Ci, --Di;
    adj[Ci].push_back(Di);
    adj[Di].push_back(Ci);
  }

  u32 components = 0;

  for (u32 i = 0; i < N; ++i) {
    if (!tag[i]) {
      dfs(i);
      ++components;
    }
  }

  return (N - 1) + (components - 1);
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
