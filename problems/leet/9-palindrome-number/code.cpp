#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isPalindrome(int x) {
    return isPalindromeNumeric(x);
    // return isPalindromeDumb(x);
  }

  bool isPalindromeDumb(int x) {
    string str = to_string(x), reversed = str;
    reverse(reversed.begin(), reversed.end());
    return str == reversed;
  }

  bool isPalindromeNumeric(int x) {
    if (x < 0)
      return false;
    int P = 1;
    while (P <= x / 10)
      P *= 10;
    while (x > 9) {
      int most = x / P, mod = x % P;
      int least = x % 10;
      if (most != least)
        return false;
      x = mod / 10;
      P = P / 100;
    }
    return P <= 1 || x == 0;
  }
};

// *****

int main() {
  return 0;
}
