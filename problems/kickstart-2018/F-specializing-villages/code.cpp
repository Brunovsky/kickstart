#include <bits/stdc++.h>

using namespace std;

// Solution according to analysis
// *****

int V, E;
vector<int> adj;
vector<int> len;
int A0, B0;

auto solve() {
  uint32_t sets = 0;

  for (int i = 1; i <= V; ++i) {
    if (i == adj[adj[i]]) ++sets;
  }

  sets /= 2;

  if (A0 > 0) {
    for (int i = 1; i <= V; ++i) {
      if (adj[i] == A0 || adj[i] == B0) ++sets;
    }
    sets -= 2;
  }

  return uint64_t(1) << sets;
}

// *****

void parse_test() {
  cin >> V >> E >> ws;
  adj.assign(V + 1, INT_MAX);
  len.assign(V + 1, INT_MAX);
  A0 = B0 = 0;

  for (int i = 0; i < E; ++i) {
    int a, b, l;
    cin >> a >> b >> l;
    if (l < len[a]) {
      adj[a] = b;
      len[a] = l;
    }
    if (l < len[b]) {
      adj[b] = a;
      len[b] = l;
    }
    if (l == 0) {
      A0 = a;
      B0 = b;
    }
  }
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 0; t < T; ++t) {
    parse_test();
    cout << "Case #" << (t + 1) << ": " << solve() << '\n';
  }
  return 0;
}
