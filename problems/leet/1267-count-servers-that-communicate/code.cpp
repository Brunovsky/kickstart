#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int countServers(vector<vector<int>> &grid) {
    int R = grid.size(), C = grid[0].size();
    vector<int> cntx(R, 0);
    vector<int> cnty(C, 0);
    for (int r = 0; r < R; r++) {
      for (int c = 0; c < C; c++) {
        if (grid[r][c]) {
          cntx[r]++;
          cnty[c]++;
        }
      }
    }
    int ans = 0;
    for (int r = 0; r < R; r++) {
      for (int c = 0; c < C; c++) {
        ans += grid[r][c] && (cntx[r] > 1 || cnty[c] > 1);
      }
    }
    return ans;
  }
};

// *****

int main() {
  return 0;
}
