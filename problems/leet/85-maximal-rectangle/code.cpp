#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int maximalRectangle(const vector<vector<char>> &matrix) {
    if (matrix.empty())
      return 0;
    int m = matrix.size(), n = matrix[0].size();
    // [left[i], right[i]) is the range with heights >= heights[i]
    vector<int> left(n, 0);
    vector<int> right(n, n);
    vector<int> heights(n, 0);

    int area = 0;
    for (int j = 0; j < m; ++j) {
      int zero = n;
      // update right[]
      for (int i = n - 1; i >= 0; --i) {
        if (matrix[j][i] == '0') {
          right[i] = n;
          zero = i;
        } else {
          right[i] = min(right[i], zero);
        }
      }

      zero = 0;
      // update heights[], left[] and area
      for (int i = 0; i < n; ++i) {
        if (matrix[j][i] == '0') {
          heights[i] = 0;
          left[i] = 0;
          zero = i + 1;
        } else {
          left[i] = max(left[i], zero);
          int h = ++heights[i], w = right[i] - left[i];
          if (area < h * w)
            area = h * w;
        }
      }
    }

    return area;
  }
};

// *****

void test() {
  vector<vector<vector<char>>> tests = {{
      {'0', '1', '1', '0', '0', '1', '0', '1', '0', '1'},
      {'0', '0', '1', '0', '1', '0', '1', '0', '1', '0'},
      {'1', '0', '0', '0', '0', '1', '0', '1', '1', '0'},
      {'0', '1', '1', '1', '1', '1', '1', '0', '1', '0'},
      {'0', '0', '1', '1', '1', '1', '1', '1', '1', '0'},
      {'1', '1', '0', '1', '0', '1', '1', '1', '1', '0'},
      {'0', '0', '0', '1', '1', '0', '0', '0', '1', '0'},
      {'1', '1', '0', '1', '1', '0', '0', '1', '1', '1'},
      {'0', '1', '0', '1', '1', '0', '1', '0', '1', '1'},
  }};

  vector<int> expected = {10};

  bool dirty = false;
  for (size_t i = 0; i < tests.size(); ++i) {
    auto actual = Solution{}.maximalRectangle(tests[i]);
    if (actual != expected[i]) {
      printf("Test #%lu failed\n", i);
      printf("> Expected: %d\n", expected[i]);
      printf("> Actual:   %d\n", actual);
      dirty = true;
    }
  }

  if (!dirty)
    cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
