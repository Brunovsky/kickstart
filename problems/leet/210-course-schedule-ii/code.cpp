#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> findOrder(int N, const vector<vector<int>> &prerequisites) {
    vector<vector<int>> out_adj(N);
    vector<int> in_adj(N);

    for (uint i = 0; i < prerequisites.size(); i++) {
      int v = prerequisites[i][0];
      int u = prerequisites[i][1];
      out_adj[u].push_back(v);
      in_adj[v]++;
    }

    vector<int> frontier;
    for (int i = 0; i < N; i++) {
      if (in_adj[i] == 0) {
        frontier.push_back(i);
      }
    }

    vector<int> ans;
    while (!frontier.empty()) {
      int u = frontier.back();
      frontier.pop_back();
      ans.push_back(u);

      for (int v : out_adj[u]) {
        in_adj[v]--;
        if (in_adj[v] == 0) {
          frontier.push_back(v);
        }
      }
    }

    if (int(ans.size()) != N) {
      ans.clear();
    }
    return ans;
  }
};

// *****

int main() {
  return 0;
}
