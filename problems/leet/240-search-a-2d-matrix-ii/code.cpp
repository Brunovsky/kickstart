#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool searchMatrix(vector<vector<int>> &matrix, int target) {
    if (matrix.empty())
      return false;
    int R = matrix.size(), C = matrix[0].size();
    int r = 0, c = C - 1;
    while (r < R && c >= 0) {
      if (matrix[r][c] == target) {
        return true;
      } else if (matrix[r][c] < target) {
        ++r;
      } else {
        --c;
      }
    }
    return false;
  }
};

// *****

int main() {
  return 0;
}
