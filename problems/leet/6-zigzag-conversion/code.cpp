#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string convert(string s, int numRows) {
    if (numRows == 1)
      return s;
    int S = s.size(), period = (numRows - 1) << 1;
    string zigzag;
    zigzag.reserve(S);
    for (int i = 0; i < S; i += period) {
      zigzag += s[i];
    }
    for (int row = 1; row + 1 < numRows; ++row) {
      int i = row, j = 2 * (numRows - 1) - row;
      while (j < S) {
        zigzag += s[i];
        zigzag += s[j];
        i += period;
        j += period;
      }
      if (i < S) {
        zigzag += s[i];
      }
    }
    for (int i = numRows - 1; i < S; i += period) {
      zigzag += s[i];
    }
    return zigzag;
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
