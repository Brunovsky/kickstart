#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int minFlipsMonoIncr(string S) {
    if (S.empty())
      return 0;
    int Z = 0, m = 0, s = S.size();
    for (int i = 0; i < s; ++i) {
      if (S[i] == '0')
        ++Z;
      m = std::min(m, i + 1 - 2 * Z);
    }
    return m + Z;
  }
};

// *****

int main() {
  return 0;
}
