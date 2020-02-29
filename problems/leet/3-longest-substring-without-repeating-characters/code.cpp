#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int lengthOfLongestSubstring(const string &s) {
    array<int, 256> indices{};
    int curlen = 0, maxlen = 0;
    for (int i = 0, S = s.size(); i < S; ++i) {
      int c = s[i] - 'a';
      ++curlen;
      if (indices[c] > 0)
        curlen = min(curlen, i + 1 - indices[c]);
      indices[c] = i + 1;
      maxlen = max(maxlen, curlen);
      printf("%d %d\n", maxlen, curlen);
    }
    printf("===\n");
    return maxlen;
  }
};

// *****

void test() {
  const vector<string> tests = {
      "abcabcbb"s,
      "bbbbb"s,
      "pwwkew"s,
  };

  const vector<int> expected = {3, 1, 3};

  bool dirty = false;
  for (size_t i = 0; i < tests.size(); ++i) {
    int actual = Solution{}.lengthOfLongestSubstring(tests[i]);
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
