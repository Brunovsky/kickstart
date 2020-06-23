#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int uniquePathsWithObstacles(const vector<vector<int>> &grid) {
    if (grid.empty())
      return 0;

    int m = grid.size(), n = grid[0].size();
    vector<unsigned> prev(n + 1, 0), next(n + 1, 0);

    prev[n - 1] = 1;

    for (int r = m - 1; r >= 0; --r) {
      for (int c = n - 1; c >= 0; --c) {
        next[c] = grid[r][c] ? 0 : prev[c] + next[c + 1];
      }
      swap(prev, next);
    }

    return int(prev[0]);
  }
};

// *****

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
