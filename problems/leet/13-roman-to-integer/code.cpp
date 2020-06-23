#include <bits/stdc++.h>

using namespace std;

// *****

// ugly af

class Solution {
public:
  int value(char c) const {
    switch (c) {
    case 'I':
      return 1;
    case 'V':
      return 5;
    case 'X':
      return 10;
    case 'L':
      return 50;
    case 'C':
      return 100;
    case 'D':
      return 500;
    case 'M':
      return 1000;
    default:
      return 0;
    }
  }
  int romanToInt(const string &s) {
    int num = 0, small = 10000;
    for (char c : s) {
      int val = value(c);
      if (val > small)
        num += val - 2 * small;
      else
        num += val, small = val;
    }
    return num;
  }
};

// *****

void test() {
  cout << Solution{}.romanToInt("MCMXCIV") << ' ' << 1994 << '\n';
  cout << Solution{}.romanToInt("LVIII") << ' ' << 58 << '\n';
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
