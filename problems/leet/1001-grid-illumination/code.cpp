#include <bits/stdc++.h>

using namespace std;

// *****

struct Point {
  int x, y;
};

bool operator==(Point lhs, Point rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

namespace std {

template <>
struct hash<Point> {
  size_t operator()(Point point) const noexcept {
    size_t x = point.x, y = point.y;
    return (x << 32) | y;
  }
};

} // namespace std

class Solution {
public:
  vector<int> gridIllumination(int N, const vector<vector<int>> &lampsvec,
                               const vector<vector<int>> &queries) {
    (void)N;

    unordered_map<int, int> X, Y, S, D;
    unordered_set<Point> lamps;

    int L = lampsvec.size();
    for (int i = 0; i < L; i++) {
      int x = lampsvec[i][0], y = lampsvec[i][1];
      X[x]++;
      Y[y]++;
      S[x + y]++;
      D[x - y]++;
      lamps.insert({x, y});
    }

    int Q = queries.size();
    vector<int> ans(Q);

    for (int q = 0; q < Q; q++) {
      int x = queries[q][0], y = queries[q][1];
      ans[q] = X[x] || Y[y] || S[x + y] || D[x - y];

      for (int lx : {x - 1, x, x + 1}) {
        for (int ly : {y - 1, y, y + 1}) {
          if (lamps.count({lx, ly})) {
            X[lx]--;
            Y[ly]--;
            S[lx + ly]--;
            D[lx - ly]--;
            lamps.erase({lx, ly});
          }
        }
      }
    }

    return ans;
  }
};

// *****

Solution sol;

int main() {
  auto ans = sol.gridIllumination(0, {{0, 0}, {4, 4}}, {{1, 1}, {1, 0}});
  assert(ans == (vector<int>{1, 0}));
  return 0;
}
