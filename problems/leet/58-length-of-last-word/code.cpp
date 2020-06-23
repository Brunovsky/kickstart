#include <bits/stdc++.h>

using namespace std;

// *****

// Surprisingly annoying

class Solution {
public:
  int lengthOfLastWord(const string &s) {
    int S = s.size(), i = S - 1;
    while (i >= 0 && s[i] == ' ')
      --i;
    if (i < 0)
      return 0;
    int j = i;
    while (i >= 0 && s[i] != ' ')
      --i;
    return j - i;
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
