#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

u32 N, M;
vector<vector<u32>> adj;
vector<i8> tag;

void dfs(u32 i) {
  tag[i] = 1;
  for (u32 j : adj[i]) {
    if (!tag[j]) dfs(j);
  }
}

auto solve() {
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

void parse_test() {
  cin >> N >> M;
  adj.resize(N);
  tag.resize(N, 0);
  for (u32 m = 0; m < M; ++m) {
    u32 Ci, Di;
    cin >> Ci >> Di >> ws;
    --Ci, --Di;
    adj[Ci].push_back(Di);
    adj[Di].push_back(Ci);
  }
}

void reset_test() {
  N = M = 0;
  adj.clear();
  tag.clear();
}

void print_test(u64 casenum, u32 solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
