#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;
using i8 = int8_t;

// *****

u32 N;
vector<vector<u32>> adj;

constexpr i8 CLEAN = 0, ENTER = 1, EXIT = 2, CYCLE = 3;

vector<i8> visited;
vector<u32> dist;
u32 cycle_root;

i8 enter_cycle(u32 i, u32 p) {
  // Detected a cycle?
  if (visited[i] == ENTER) {
    cycle_root = i;
    return CYCLE;
  }

  assert(visited[i] == CLEAN);
  visited[i] = ENTER;

  for (u32 j : adj[i]) {
    if (j == p)
      continue;
    i8 command = enter_cycle(j, i);
    if (command == CYCLE) {
      visited[i] = CLEAN;
      dist[i] = 0;
      return (i == cycle_root) ? EXIT : CYCLE;
    } else if (command == EXIT) {
      visited[i] = CLEAN;
      return EXIT;
    }
  }

  visited[i] = CLEAN;
  return CLEAN;
}

void enter_dist(u32 i, u32 p) {
  if (visited[i] != CLEAN)
    return;
  visited[i] = ENTER;

  for (u32 j : adj[i]) {
    if (j == p)
      continue;
    if (visited[j] == CLEAN) {
      dist[j] = min(dist[i] + 1, dist[j]);
      enter_dist(j, i);
    }
  }
}

void solve() {
  cin >> N >> ws;
  adj.assign(N, {});
  for (u32 n = 0; n < N; ++n) {
    u32 xi, yi;
    cin >> xi >> yi >> ws;
    --xi, --yi;
    adj[xi].push_back(yi);
    adj[yi].push_back(xi);
  }

  visited.assign(N, CLEAN);
  dist.assign(N, N);

  enter_cycle(0, 0);

  for (u32 i = 0; i < N; ++i) {
    if (dist[i] == 0) {
      enter_dist(i, i);
      break;
    }
  }
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    solve();
    cout << "Case #" << t << ": ";
    for (u32 d : dist)
      cout << ' ' << d;
    cout << '\n';
  }
  return 0;
}
