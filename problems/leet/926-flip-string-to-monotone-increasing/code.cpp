#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int minFlipsMonoIncr(string S) {
    if (S.empty())
      return 0;
    int Z = 0, m = 0;
    for (int i = 0; i < S.size(); ++i) {
      if (S[i] == '0')
        ++Z;
      m = std::min(m, i + 1 - 2 * Z);
    }
    return m + Z;
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
