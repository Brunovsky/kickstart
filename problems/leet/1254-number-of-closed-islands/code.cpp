#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int dd[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

class Solution {
  int R, C;

  bool floodfill(vector<vector<int>> &grid, int sr, int sc) {
    assert(grid[sr][sc] == 0);
    vector<pair<int, int>> frontier;
    frontier.push_back({sr, sc});
    grid[sr][sc] = 2;

    bool overflow = false;
    while (!frontier.empty()) {
      auto [r, c] = frontier.back();
      frontier.pop_back();

      for (auto d : dd) {
        int dr = r + d[0], dc = c + d[1];
        if (dr >= 0 && dr < R && dc >= 0 && dc < C) {
          if (grid[dr][dc] == 0) {
            frontier.push_back({dr, dc});
            grid[dr][dc] = 2;
          }
        } else {
          overflow = true;
        }
      }
    }
    return !overflow;
  }

public:
  int closedIsland(vector<vector<int>> &grid) {
    R = grid.size();
    if (R == 0)
      return 0;
    C = grid[0].size();
    int cnt = 0;
    for (int r = 0; r < R; r++) {
      for (int c = 0; c < C; c++) {
        if (grid[r][c] == 0) {
          cnt += floodfill(grid, r, c);
        }
      }
    }
    return cnt;
  }
};

// *****

int main() {
  return 0;
}
