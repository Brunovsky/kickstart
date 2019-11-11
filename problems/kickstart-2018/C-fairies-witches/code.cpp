#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;
using u64 = uint64_t;

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

void reparse_test() {
  cin >> N;
  M.assign(N, vector<u32>(N, 0));
  for (u32 i = 0; i < N; ++i) {
    for (u32 j = 0; j < N; ++j) {
      cin >> M[i][j];
    }
  }
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
