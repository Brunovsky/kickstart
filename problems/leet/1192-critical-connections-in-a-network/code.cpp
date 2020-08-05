#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<vector<int>>
  criticalConnections(int n, const vector<vector<int>> &connections) {
    int m = connections.size();
    vector<vector<int>> adj(n);
    vector<int> source(m);
    vector<int> target(m);

    for (int e = 0; e < m; e++) {
      int u = connections[e][0], v = connections[e][1];
      adj[u].push_back(e), adj[v].push_back(e);
      source[e] = u, target[e] = v;
    }

    constexpr int inf = INT_MAX;
    vector<int> dist(n);
    vector<bool> loop(m, false);

    using dfs_t = function<int(int, int)>;
    dfs_t dfs = [&](int u, int depth) {
      if (dist[u] != 0)
        return dist[u];
      dist[u] = depth;
      int closest_all = inf;
      for (int e : adj[u]) {
        int v = u ^ source[e] ^ target[e];
        if (dist[u] - dist[v] == 1)
          continue;
        int closest = dfs(v, depth + 1);
        closest_all = min(closest, closest_all);
        if (closest <= depth) {
          loop[e] = true;
        }
      }
      return closest_all;
    };

    dfs(0, 2);

    vector<vector<int>> ans;
    for (int e = 0; e < m; e++) {
      if (!loop[e]) {
        ans.push_back({source[e], target[e]});
      }
    }
    return ans;
  }
};

// *****

void print(int t, const vector<vector<int>> &adj) {
  printf("%d: [", t);
  if (!adj.empty()) {
    printf("[%d,%d]", adj[0][0], adj[0][1]);
    for (uint i = 1; i < adj.size(); i++) {
      printf(", [%d,%d]", adj[i][0], adj[i][1]);
    }
  }
  printf("]\n");
}

int main() {
  vector<int> testn = {4, 6};
  vector<vector<vector<int>>> testc = {
      {{0, 1}, {1, 2}, {2, 0}, {1, 3}},
      {{0, 1}, {1, 2}, {2, 0}, {1, 3}, {3, 4}, {4, 5}, {5, 3}},
  };

  for (uint i = 0; i < testn.size(); i++) {
    auto ans = Solution{}.criticalConnections(testn[i], testc[i]);
    print(i, ans);
  }

  return 0;
}
