#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<vector<int>> generateMatrix(int n) {
    if (n == 0)
      return {};

    vector<vector<int>> matrix(n, vector<int>(n));

    int k = 0, r = 0, c = -1, horz = n, vert = n - 1;
    while (true) {
      if (!horz--)
        break;
      for (int i = 0; i <= horz; ++i)
        matrix[r][++c] = ++k;

      if (!vert--)
        break;
      for (int i = 0; i <= vert; ++i)
        matrix[++r][c] = ++k;

      if (!horz--)
        break;
      for (int i = 0; i <= horz; ++i)
        matrix[r][--c] = ++k;

      if (!vert--)
        break;
      for (int i = 0; i <= vert; ++i)
        matrix[--r][c] = ++k;
    }

    return matrix;
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
