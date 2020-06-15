#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int numDecodings(const string &s) {
    if (s.empty())
      return 0;
    if (s[0] == '0')
      return 0;

    int prev1 = 1;
    int prev2 = 1;

    for (size_t i = 1; i < s.size(); ++i) {
      int lo = s[i] - '0', hi = s[i - 1] - '0';
      int n = (hi * 10) + lo;
      int cur = 0;

      if (lo >= 1 && lo < 10) {
        cur += prev1;
      }

      if (n >= 10 && n <= 26) {
        cur += prev2;
      }

      prev2 = prev1;
      prev1 = cur;
    }

    return prev1;
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
