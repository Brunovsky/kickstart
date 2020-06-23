#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int strStr(const string &haystack, const string &needle) {
    int H = haystack.size(), N = needle.size();
    for (int i = 0; i + N < H + 1; ++i) {
      for (int j = 0; j < N; ++j)
        if (needle[j] != haystack[i + j])
          goto outer;
      return i;
    outer:
      continue;
    }
    return -1;
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
