#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  void setZeroes(vector<vector<int>> &matrix) {
    size_t n = matrix.size(), m = matrix[0].size();
    bool i0 = false, j0 = false;
    for (size_t i = 0; i < n; ++i) {
      if (matrix[i][0])
        continue;
      j0 = true;
      break;
    }
    for (size_t j = 0; j < m; ++j) {
      if (matrix[0][j])
        continue;
      i0 = true;
      break;
    }
    for (size_t i = 1; i < n; ++i) {
      for (size_t j = 1; j < m; ++j) {
        if (matrix[i][j])
          continue;
        matrix[0][j] = matrix[i][0] = 0;
      }
    }
    for (size_t i = 1; i < n; ++i) {
      if (matrix[i][0])
        continue;
      for (size_t j = 1; j < m; ++j)
        matrix[i][j] = 0;
    }
    for (size_t j = 1; j < m; ++j) {
      if (matrix[0][j])
        continue;
      for (size_t i = 1; i < n; ++i)
        matrix[i][j] = 0;
    }
    if (j0)
      for (size_t i = 0; i < n; ++i)
        matrix[i][0] = 0;
    if (i0)
      for (size_t j = 0; j < m; ++j)
        matrix[0][j] = 0;
  }
};

// *****

int main() {
  return 0;
}
