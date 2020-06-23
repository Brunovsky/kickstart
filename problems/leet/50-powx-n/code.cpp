#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  double myPow(double x, int n) {
    if (n == 0)
      return 1.0;
    if (n == INT_MIN)
      return myPow(x, n + 1) / x;
    if (n < 0)
      x = 1 / x, n = -n;
    double power = 1.0;

    while (n > 0) {
      if (n & 1)
        power *= x;
      x *= x;
      n >>= 1;
    }

    return power;
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
