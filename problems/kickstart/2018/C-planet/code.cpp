#include <bits/stdc++.h>

using namespace std;
using u32 = uint32_t;
using i8 = int8_t;

// *****

u32 N;
vector<vector<u32>> adj;

constexpr i8 CLEAN = 0, ENTER = 1, EXIT = 2, CYCLE = 3;

vector<i8> visit;
vector<u32> dist;
u32 cycle_root;

i8 enter_cycle(u32 i, u32 p) {
  // Detected a cycle?
  if (visit[i] == ENTER) {
    cycle_root = i;
    return CYCLE;
  }

  assert(visit[i] == CLEAN);
  visit[i] = ENTER;

  for (u32 j : adj[i]) {
    if (j == p) continue;
    i8 command = enter_cycle(j, i);
    if (command == CYCLE) {
      visit[i] = CLEAN;
      dist[i] = 0;
      return (i == cycle_root) ? EXIT : CYCLE;
    } else if (command == EXIT) {
      visit[i] = CLEAN;
      return EXIT;
    }
  }

  visit[i] = CLEAN;
  return CLEAN;
}

void enter_dist(u32 i, u32 p) {
  if (visit[i] != CLEAN) return;
  visit[i] = ENTER;

  for (u32 j : adj[i]) {
    if (j == p) continue;
    if (visit[j] == CLEAN) {
      dist[j] = min(dist[i] + 1, dist[j]);
      enter_dist(j, i);
    }
  }
}

const vector<u32>& solve() {
  visit = vector<i8>(N, CLEAN);
  dist = vector<u32>(N, N);

  enter_cycle(0, 0);

  for (u32 i = 0; i < N; ++i) {
    if (dist[i] == 0) {
      enter_dist(i, i);
      break;
    }
  }

  return dist;
}

// *****

void reparse_test() {
  cin >> N >> ws;
  adj.assign(N, {});
  for (u32 n = 0; n < N; ++n) {
    u32 xi, yi;
    cin >> xi >> yi >> ws;
    --xi, --yi;
    adj[xi].push_back(yi);
    adj[yi].push_back(xi);
  }
}

void print_test(const vector<u32>& solution) {
  for (u32 d : solution) cout << ' ' << d;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": ";
    print_test(solution);
    cout << '\n';
  }
  return 0;
}
