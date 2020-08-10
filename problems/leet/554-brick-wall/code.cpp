#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int leastBricks(vector<vector<int>> &wall) {
    unordered_map<int, uint> cnt;
    for (uint i = 0; i < wall.size(); i++) {
      uint sum = 0;
      for (uint j = 0; j + 1 < wall[i].size(); j++) {
        sum += wall[i][j];
        cnt[sum]++;
      }
    }
    uint max_cnt = 0;
    for (auto [_, n] : cnt)
      max_cnt = max(max_cnt, n);
    return wall.size() - max_cnt;
  }
};

// *****

int main() {
  return 0;
}
