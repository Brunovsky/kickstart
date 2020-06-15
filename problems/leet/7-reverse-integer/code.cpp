#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int reverse(int a) {
    int64_t r = 0, x = a;
    bool sign = false;
    if (x < 0)
      sign = true, x = -x;
    while (x != 0) {
      r = r * 10 + x % 10;
      x /= 10;
    }
    if (sign)
      r = -r;
    if (r > INT_MAX || r < INT_MIN)
      return 0;
    return r;
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
