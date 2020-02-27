#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  void dive(int n, int l, int r, string &text, vector<string> &sol) const {
    if (l < n) {
      text.push_back('(');
      dive(n, l + 1, r, text, sol);
      text.pop_back();
    }
    if (r < n && l > r) {
      text.push_back(')');
      dive(n, l, r + 1, text, sol);
      text.pop_back();
    }
    if (l == n && r == n) {
      sol.push_back(text);
    }
  }

public:
  vector<string> generateParenthesis(int n) {
    vector<string> sol;
    string text;

    sol.reserve(1 << n);
    text.reserve(2 * n);

    dive(n, 0, 0, text, sol);
    return sol;
  }
};

// *****

void test() {}

// *****

int main() {
  test();
  return 0;
}
