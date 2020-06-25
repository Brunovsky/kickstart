#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool searchMatrix(const vector<vector<int>> &matrix, int target) {
    if (matrix.empty())
      return false;
    int M = matrix.size(), n = matrix[0].size();

    int l = 0, r = M * n - 1;
    while (l <= r) {
      int m = (l + r) / 2;
      int val = matrix[m / n][m % n];
      if (target < val) {
        r = m - 1;
      } else if (target > val) {
        l = m + 1;
      } else {
        return true;
      }
    }
    return false;
  }
};

// *****

int main() {
  return 0;
}
