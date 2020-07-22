#include <bits/stdc++.h>

using namespace std;

// *****

using map_t = unordered_map<unsigned long, int>;
using ulong = unsigned long;

class Solution {
public:
  // naive
  int maxPoints(const vector<vector<int>> &points) {
    int most = 1, P = points.size();
    if (P <= 2)
      return P;
    for (int i = 0; i < P; ++i) {
      int overlap = 1;
      map_t count;
      for (int j = 0; j < P; ++j) {
        if (i == j) {
          continue;
        }
        int x = points[i][0] - points[j][0];
        int y = points[i][1] - points[j][1];
        if (x == 0 && y == 0) {
          ++overlap;
          continue;
        }
        if (y < 0) {
          x = -x;
          y = -y;
        }
        if (y == 0) {
          x = 1;
        }
        int g = std::gcd(x, y);
        assert(g > 0);
        x /= g, y /= g;
        ulong mask = (ulong(x) << 32) | ulong(y);
        count[mask]++;
      }
      for (auto it = count.begin(); it != count.end(); ++it) {
        if (most < it->second + overlap)
          most = it->second + overlap;
      }
      if (most < overlap)
        most = overlap;
    }
    return most;
  }
};

// *****

int main() {
  const vector<vector<vector<int>>> tests = {
      {{1, 1}, {2, 2}, {3, 3}},
      {{1, 1}, {3, 2}, {5, 3}, {4, 1}, {2, 3}, {1, 4}},
      {{0, 0}, {0, 0}},
      {{0, 0}, {1, 1}, {0, 0}},
      {{4, 5}, {4, -1}, {4, 0}},
      {{0, -1},
       {0, 3},
       {0, -4},
       {0, -2},
       {0, -4},
       {0, 0},
       {0, 0},
       {0, 1},
       {0, -2},
       {0, 4}},
  };

  const vector<int> expected = {3, 4, 2, 3, 3, 10};

  for (size_t i = 0; i < tests.size(); ++i) {
    int actual = Solution{}.maxPoints(tests[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %d\n", expected[i]);
    printf("> Actual:   %d\n", actual);
  }

  return 0;
}
