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

int main() {
  return 0;
}
