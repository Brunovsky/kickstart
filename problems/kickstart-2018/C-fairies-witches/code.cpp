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

u32 N;
vector<vector<u32>> M;

struct edge_t {
  u32 i, j;

  friend bool operator<(edge_t lhs, edge_t rhs) noexcept {
    return M[lhs.i][lhs.j] < M[rhs.i][rhs.j];
  }
};

u64 visit(vector<edge_t> edges, edge_t e0, u64 minsum) {
  auto newbegin = remove_if(edges.begin(), edges.end(), [e0](edge_t edge) {
    return edge.i == e0.i || edge.j == e0.i || edge.i == e0.j || edge.j == e0.j;
  });
  edges.erase(newbegin, edges.end());

  u64 solutions = 0;

  while (!edges.empty()) {
    edge_t e = edges.back();
    edges.pop_back();

    bool sufficient = M[e.i][e.j] >= minsum;
    if (sufficient) ++solutions;

    u64 newminsum = sufficient ? 0 : minsum - M[e.i][e.j];
    solutions += visit(edges, e, newminsum);
  }

  return solutions;
}

auto solve() {
  vector<edge_t> edges;

  for (u32 i = 0; i < N; ++i) {
    for (u32 j = i + 1; j < N; ++j) {
      if (M[i][j] > 0) edges.push_back({i, j});
    }
  }

  sort(edges.begin(), edges.end());

  u64 solution = 0;

  while (!edges.empty()) {
    edge_t e = edges.back();
    edges.pop_back();

    solution += visit(edges, e, M[e.i][e.j] + 1);
  }

  return solution;
}

// *****

void prep() {}

void parse_test() {
  cin >> N;
  M.resize(N, vector<u32>(N, 0));
  for (u32 i = 0; i < N; ++i) {
    for (u32 j = 0; j < N; ++j) {
      cin >> M[i][j];
    }
  }
}

void reset_test() {
  M.clear();
}

void print_test(u64 casenum, u64 solution) {
  cout << "Case #" << casenum << ": ";
  cout << solution;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  prep();
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
