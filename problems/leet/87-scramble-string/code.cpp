#include <bits/stdc++.h>

using namespace std;

// *****

constexpr size_t ASCII = 'Z' - 'A' + 1;
using alph = array<uint16_t, ASCII>;

class Solution {
  alph sub(const alph &lhs, const alph &rhs) const {
    alph diff{};
    for (size_t i = 0; i < ASCII; ++i)
      diff[i] = lhs[i] - rhs[i];
    return diff;
  }

  bool isScramble(const string &s1, const string &s2, const vector<alph> &a1,
                  const vector<alph> &a2, int i1, int i2, int len) {
    if (len <= 3)
      return true;

    for (int i = 1; i < len; ++i) {
      const auto left1 = sub(a1[i1 + i], a1[i1]);
      const auto left2 = sub(a2[i2 + i], a2[i2]);
      const auto right2 = sub(a2[i2 + len], a2[i2 + len - i]);

      // current node's children don't swap
      if (left1 == left2) {
        if (isScramble(s1, s2, a1, a2, i1, i2, i) &&
            isScramble(s1, s2, a1, a2, i1 + i, i2 + i, len - i))
          return true;
      }

      // current node's children swap
      if (left1 == right2) {
        if (isScramble(s1, s2, a1, a2, i1, i2 + len - i, i) &&
            isScramble(s1, s2, a1, a2, i1 + i, i2, len - i))
          return true;
      }
    }
    return false;
  }

public:
  bool isScramble(const string &s1, const string &s2) {
    if (s1.length() != s2.length())
      return false;

    size_t n = s1.length();
    vector<alph> a1(n + 1), a2(n + 1);

    for (size_t i = 1; i < n + 1; ++i) {
      a1[i] = a1[i - 1];
      a1[i][s1[i - 1] - 'a']++;
      a2[i] = a2[i - 1];
      a2[i][s2[i - 1] - 'a']++;
    }

    if (a1[n] != a2[n])
      return false;

    return isScramble(s1, s2, a1, a2, 0, 0, n);
  }
};

// *****

void test() {
  Solution solver;
  {
    const string s1 = "great", s2 = "rgate";
    bool is = solver.isScramble(s1, s2);
    if (!is)
      cout << "great test failed";
  }
  {
    const string s1 = "abcde", s2 = "caebd";
    bool is = solver.isScramble(s1, s2);
    if (is)
      cout << "abcde test failed";
  }
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
