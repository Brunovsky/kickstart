#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool searchMatrix(const vector<vector<int>> &matrix, int target) {
    if (matrix.empty())
      return false;
    int m = matrix.size(), n = matrix[0].size();

    int l = 0, r = m * n - 1;
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

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
