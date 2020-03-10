#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int lengthLongestPath(string input) {
    vector<int> lengths(10, 0);

    int maxlen = 0;
    for (size_t i = 0; i < input.size(); ++i) {
      int tabs = 0;
      int namelen = 0;
      bool dot = false;
      while (i < input.size() && input[i] == '\t')
        ++tabs, ++i;
      while (i < input.size() && input[i] != '\n') {
        if (input[i] == '.')
          dot = true;
        ++namelen, ++i;
      }
      if (lengths.size() < tabs + 1)
        lengths.resize(tabs + 1, 0);
      lengths[tabs] = namelen;
      if (tabs > 0)
        lengths[tabs] += lengths[tabs - 1] + 1;
      if (dot)
        maxlen = max(maxlen, lengths[tabs]);
    }
    return maxlen;
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
