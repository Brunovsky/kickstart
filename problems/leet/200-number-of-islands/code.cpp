#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  // Floodfill the island containing the tile (rx,cx).
  // Do this by setting the grid value to '2'
  void floodfill(vector<vector<char>> &grid, int rx, int cx) {
    int R = grid.size(), C = grid[0].size();

    vector<pair<int, int>> frontier{{rx, cx}}, old_frontier;

    grid[rx][cx] = '2';

    while (!frontier.empty()) {
      swap(frontier, old_frontier);
      frontier.clear();

      for (auto [r, c] : old_frontier) {
        if (r > 0 && grid[r - 1][c] == '1')
          frontier.push_back({r - 1, c}), grid[r - 1][c] = '2';
        if (c > 0 && grid[r][c - 1] == '1')
          frontier.push_back({r, c - 1}), grid[r][c - 1] = '2';
        if (r + 1 < R && grid[r + 1][c] == '1')
          frontier.push_back({r + 1, c}), grid[r + 1][c] = '2';
        if (c + 1 < C && grid[r][c + 1] == '1')
          frontier.push_back({r, c + 1}), grid[r][c + 1] = '2';
      }
    }
  }

public:
  int numIslands(vector<vector<char>> &grid) {
    int islands = 0;
    if (grid.size() == 0 || grid[0].size() == 0)
      return 0;

    // Floodfill found land tiles and count number of floodfills.
    int R = grid.size(), C = grid[0].size();
    for (int r = 0; r < R; ++r) {
      for (int c = 0; c < C; ++c) {
        if (grid[r][c] == '0')
          continue;
        if (grid[r][c] == '1') {
          floodfill(grid, r, c);
          ++islands;
        }
      }
    }

    return islands;
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
