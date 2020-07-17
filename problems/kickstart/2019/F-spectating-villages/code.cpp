#include <bits/stdc++.h>

using namespace std;
using i64 = int64_t;

// *****

int V;
vector<i64> B;           // beauty
vector<vector<int>> adj; // neighbours

vector<i64> parent;  // L on parent -> u illuminated
vector<i64> current; // L on u -> u + children illuminated
vector<i64> none;    // no L on u or children -> u not illuminated
vector<i64> exists;  // no L on u, L on at least 1 children -> u illuminated

void visit(int u, int ancestor) {
  parent[u] = B[u];
  current[u] = B[u];
  none[u] = 0;
  exists[u] = B[u];

  i64 lighthouse = LONG_MIN;
  int count = 0;

  for (int v : adj[u]) {
    if (v == ancestor)
      continue;
    visit(v, u);

    i64 cell = max(current[v], max(none[v], exists[v]));
    lighthouse = max(lighthouse, current[v] - cell);
    parent[u] += cell;
    current[u] += max(current[v], parent[v]);
    none[u] += max(none[v], exists[v]);
    exists[u] += cell;

    ++count;
  }

  if (count) {
    exists[u] += lighthouse;
  } else {
    exists[u] = 0;
  }
}

auto solve() {
  cin >> V >> ws;
  B.resize(V + 1);
  adj.assign(V + 1, {});
  for (int i = 1; i <= V; i++) {
    cin >> B[i] >> ws;
  }
  for (int i = 0; i < V - 1; i++) {
    int u, v;
    cin >> u >> v >> ws;
    adj[u].push_back(v);
    adj[v].push_back(u);
  }
  parent.assign(V + 1, 0);
  current.assign(V + 1, 0);
  none.assign(V + 1, 0);
  exists.assign(V + 1, 0);
  visit(1, 1);
  return max(current[1], max(none[1], exists[1]));
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
