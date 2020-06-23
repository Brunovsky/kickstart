#include <bits/stdc++.h>

using namespace std;

// *****

inline char flip(char c) {
  switch (c) {
  case ')':
    return '(';
  case '}':
    return '{';
  case ']':
    return '[';
  default:
    return c;
  }
}

class Solution {
public:
  bool isValid(const string &s) {
    string stack;
    stack.reserve(s.size() / 2);
    for (char c : s) {
      if (c == '(' || c == '{' || c == '[') {
        stack += c;
        continue;
      }
      if (stack.empty() || stack.back() != flip(c))
        return false;
      stack.pop_back();
    }
    return stack.empty();
  }
};

// *****

void test() {
  vector<string> tests = {"()"s, "()[]{}"s, "(]"s, "([)]"s, "{[]}"s};
  vector<bool> expected = {true, true, false, false, true};

  bool dirty = false;
  for (size_t i = 0; i < tests.size(); ++i) {
    int actual = Solution{}.isValid(tests[i]);
    if (actual != expected[i]) {
      printf("Test #%lu failed\n", i);
      printf("> String: '%s'\n", tests[i].c_str());
      printf("> Expected: %s\n", expected[i] ? "true" : "false");
      printf("> Actual:   %s\n", actual ? "true" : "false");
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
