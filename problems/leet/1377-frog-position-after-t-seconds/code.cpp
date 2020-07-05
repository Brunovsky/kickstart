#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  double visit(const vector<vector<int>> &adj, int prev, int u, int t,
               int target) {
    if (t == 0) {
      return u == target ? 1.0 : 0.0;
    }
    double p = 0.0;
    int visited = 0;
    for (int v : adj[u]) {
      if (v == prev) {
        continue;
      }
      if (p <= 0.0) {
        p = visit(adj, u, v, t - 1, target);
      }
      ++visited;
    }
    if (visited == 0) {
      return u == target ? 1.0 : 0.0;
    } else {
      return p / double(visited);
    }
  }

public:
  double frogPosition(int n, vector<vector<int>> &edges, int t, int target) {
    vector<vector<int>> adj(n + 1);
    for (const auto &edge : edges) {
      int u = edge[0], v = edge[1];
      adj[u].push_back(v);
      adj[v].push_back(u);
    }
    return visit(adj, 0, 1, t, target);
  }
};

// *****

int main() {
  return 0;
}
