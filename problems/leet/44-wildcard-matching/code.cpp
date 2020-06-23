#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isMatch(const string &text, const string &pattern) {
    int T = text.size(), P = pattern.size();

    vector<uint8_t> prev(T + 1, false), next = prev;
    prev[0] = true;

    for (int j = 0; j < P; ++j) {
      char p = pattern[j];

      next[0] = prev[0] && (p == '*');

      if (p == '?')
        for (int i = 1; i <= T; ++i)
          next[i] = prev[i - 1];
      else if (p == '*')
        for (int i = 1; i <= T; ++i)
          next[i] = next[i - 1] || prev[i];
      else /* if (p == a-z) */
        for (int i = 1; i <= T; ++i)
          next[i] = prev[i - 1] && (text[i - 1] == p);

      swap(prev, next);
    }

    return prev[T];
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
