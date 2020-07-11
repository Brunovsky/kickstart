#include <bits/stdc++.h>

using namespace std;

// *****

constexpr bool IS_TOK(const string &token) {
  if (token.length() != 1)
    return false;
  char c = token[0];
  return c == '+' || c == '-' || c == '*' || c == '/';
}

class Solution {
public:
  int evalRPN(vector<string> &tokens) {
    vector<int> stack((tokens.size() + 1) / 2);
    int s = 0;
    for (int i = 0; i < tokens.size(); ++i) {
      string tok = tokens[i];
      if (IS_TOK(tok)) {
        int a = stack[s - 2];
        int b = stack[s - 1];
        char c = tok[0];
        s--;
        switch (c) {
        case '+':
          stack[s - 1] = a + b;
          break;
        case '-':
          stack[s - 1] = a - b;
          break;
        case '*':
          stack[s - 1] = a * b;
          break;
        case '/':
          stack[s - 1] = a / b;
          break;
        }
      } else {
        stack[s++] = atoi(tok.c_str());
      }
    }
    assert(s == 1);
    return stack[0];
  }
};

// *****

int main() {
  return 0;
}
