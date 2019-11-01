#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

class Solution {
  void dive(int n, int l, int r, string& text, vector<string>& sol) const {
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
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  test();
  return 0;
}
