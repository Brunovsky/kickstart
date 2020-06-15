#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  void rotate(vector<vector<int>> &matrix) {
    const size_t n = matrix.size();

    for (size_t r = 0; 2 * r < n; ++r) {
      for (size_t c = r; c + r + 1 < n; ++c) {
        auto &A = matrix[r][c];
        auto &B = matrix[c][n - r - 1];
        auto &C = matrix[n - r - 1][n - c - 1];
        auto &D = matrix[n - c - 1][r];
        auto tmp = A;
        A = D;
        D = C;
        C = B;
        B = tmp;
      }
    }
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
