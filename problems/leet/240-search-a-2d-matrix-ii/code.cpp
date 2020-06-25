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

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
