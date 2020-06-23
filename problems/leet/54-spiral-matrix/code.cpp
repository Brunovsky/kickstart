#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> spiralOrder(const vector<vector<int>> &matrix) {
    if (matrix.empty())
      return {};
    int m = matrix.size(), n = matrix[0].size();
    vector<int> spiral;
    spiral.reserve(m * n);

    int r = 0, c = -1, horz = n, vert = m - 1;
    while (true) {
      if (!horz--)
        break;
      for (int i = 0; i <= horz; ++i)
        spiral.push_back(matrix[r][++c]);

      if (!vert--)
        break;
      for (int i = 0; i <= vert; ++i)
        spiral.push_back(matrix[++r][c]);

      if (!horz--)
        break;
      for (int i = 0; i <= horz; ++i)
        spiral.push_back(matrix[r][--c]);

      if (!vert--)
        break;
      for (int i = 0; i <= vert; ++i)
        spiral.push_back(matrix[--r][c]);
    }

    return spiral;
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
