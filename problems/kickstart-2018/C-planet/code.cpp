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

void parse_test() {
  cin >> N >> ws;
  adj.resize(N);
  for (u32 n = 0; n < N; ++n) {
    u32 xi, yi;
    cin >> xi >> yi >> ws;
    --xi, --yi;
    adj[xi].push_back(yi);
    adj[yi].push_back(xi);
  }
}

void reset_test() {
  adj.clear();
}

void print_test(u64 casenum, const vector<u32>& solution) {
  cout << "Case #" << casenum << ":";
  for (u32 d : solution) cout << ' ' << d;
  cout << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  u64 T;
  cin >> T >> ws;
  for (u64 t = 0; t < T; ++t) {
    parse_test();
    print_test(t + 1, solve());
    reset_test();
  }
  return 0;
}
