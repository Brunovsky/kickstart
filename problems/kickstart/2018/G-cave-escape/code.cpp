#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int OBSTACLE = -100000;
constexpr int EMPTY = 0;

inline bool is_obstacle(int v) {
  return v == -100000;
}

inline bool is_empty(int v) {
  return v == 0;
}

inline bool is_trap(int v) {
  return v > -100000 && v < 0;
}

inline bool is_energy(int v) {
  return v > 0;
}

struct cell_t {
  int n, m;
};

inline bool operator<(cell_t lhs, cell_t rhs) {
  return lhs.n < rhs.n || (lhs.n == rhs.n && lhs.m < rhs.m);
}

// *****

int N, M;
int E;

vector<vector<int>> cave;
cell_t start, door;
vector<cell_t> traps;

set<cell_t> make_ok_traps(int mask) {
  set<cell_t> ok_traps;
  for (size_t i = 0; i < traps.size(); ++i, mask >>= 1) {
    if (mask & 1)
      ok_traps.insert(traps[i]);
  }
  return ok_traps;
}

void bfs() {}

auto solve() {
  vector<vector<bool>> vis(N + 2, vector<bool>(M + 2, false));
  set<cell_t> ok_traps;

  int mask = -1, max_mask = (1 << traps.size());

  while (++mask < max_mask) {
    ok_traps = make_ok_traps(mask);
  }

  return 0;
}

// *****

void make_walls() {
  for (int n = 0; n < N + 2; ++n) {
    cave[n][0] = cave[n][M + 1] = OBSTACLE;
  }

  for (int m = 0; m < M + 2; ++m) {
    cave[0][m] = cave[N + 1][m] = OBSTACLE;
  }
}

void reparse_test() {
  cin >> N >> M >> E >> start.n >> start.m >> door.n >> door.m >> ws;
  cave.assign(N + 2, vector<int>(M + 2, 0));
  make_walls();
  for (int n = 1; n <= N; ++n) {
    for (int m = 1; n <= M; ++m) {
      cin >> cave[n][m] >> ws;
      if (is_trap(cave[n][m]))
        traps.push_back({n, m});
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
