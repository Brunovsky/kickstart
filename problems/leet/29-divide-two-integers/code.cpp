#include <bits/stdc++.h>

using namespace std;

// *****

// -_-

class Solution {
public:
  int divide(int dividend, int divisor) {
    if (divisor == 1)
      return dividend;
    if (divisor == -1)
      return dividend == INT_MIN ? INT_MAX : -dividend;
    bool sign = ((dividend < 0) + (divisor < 0)) & 1;
    long num = dividend < 0 ? -long(dividend) : dividend;
    long den = divisor < 0 ? -long(divisor) : divisor;
    long quotient = 0;
    long n = 1;
    long part = den;
    while ((part << 1) <= num) {
      part <<= 1, n <<= 1;
    }
    while (n) {
      if (part <= num) {
        num -= part;
        quotient += n;
      }
      part >>= 1, n >>= 1;
    }
    return sign ? -quotient : quotient;
  }
};

// *****

void test() {
  vector<int> dividend{12, 10, 10, INT_MIN, INT_MAX, INT_MAX, 73, 4};
  vector<int> divisor{3, 3, -3, 1, -1, 1, 4, 1};

  bool dirty = false;
  for (size_t i = 0; i < dividend.size(); ++i) {
    int div = Solution{}.divide(dividend[i], divisor[i]);
    if (div != dividend[i] / divisor[i]) {
      printf("Failed test %d/%d\n", dividend[i], divisor[i]);
      printf("Expected: %d\n", dividend[i] / divisor[i]);
      printf("Actual: %d\n", div);
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
