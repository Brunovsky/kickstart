#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int dd[8][2] = {{1, 0},   {-1, 0}, {0, -1}, {0, 1},
                          {-1, -1}, {-1, 1}, {1, -1}, {1, 1}};

class Solution {
public:
  vector<vector<int>> queensAttacktheKing(vector<vector<int>> &queens,
                                          vector<int> &king) {
    bool Q[8][8] = {};
    for (uint i = 0; i < queens.size(); i++) {
      Q[queens[i][0]][queens[i][1]] = true;
    }
    vector<vector<int>> ans;
    for (auto d : dd) {
      int i = 1;
      while (true) {
        int r = king[0] + i * d[0];
        int c = king[1] + i * d[1];
        i++;
        if (r < 0 || c < 0 || r >= 8 || c >= 8) {
          break;
        }
        if (Q[r][c]) {
          ans.push_back({r, c});
          break;
        }
      }
    }
    return ans;
  }
};

// *****

int main() {
  return 0;
}
