#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int longestValidParentheses(const string &s) {
    int S = s.size(), longest = 0;

    while (S > 0 && s[S - 1] == '(')
      --S;

    int l = 0;
    while (true) {
      while (l < S && s[l] == ')')
        ++l;
      if (l == S)
        break;

      int r = l + 1, r_height = 1;
      while (r < S && r_height >= 0) {
        r_height += s[r++] == '(' ? 1 : -1;
      }

      if (r_height == -1) {
        if (longest < r - l - 1)
          longest = r - l - 1;
        l = r;
      } else if (r_height == 0) {
        if (longest < r - l)
          longest = r - l;
        return longest;
      } else {
        break;
      }
    }

    int r = S;
    while (true) {
      while (r > l && s[r - 1] == '(')
        --r;
      if (r == l)
        break;

      int m = r - 1, m_height = 1;
      while (m > l && m_height >= 0) {
        m_height += s[--m] == ')' ? 1 : -1;
      }

      if (m_height == -1) {
        if (longest < r - m - 1)
          longest = r - m - 1;
        r = m;
      } else if (m_height == 0) {
        if (longest < r - l)
          longest = r - l;
        return longest;
      } else {
        break;
      }
    }

    return longest;
  }
};

// *****

/**
 *      /\  /\              /\  /\  /\
 * 0   /  \/  \            /  \/  \/  \    20
 * \  /        \      /\  /            \  /
 *  \/          \    /  \/              \/
 *   1           \  /    11              19
 *                \/   ^----------------^
 *                 8
 */

void test() {
  Solution S;
  int res = S.longestValidParentheses(")(()()))(()(()()())(");
  cout << res << endl;
}

// *****

int main() {
  test();
  return 0;
}
