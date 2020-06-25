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

int main() {
  return 0;
}
