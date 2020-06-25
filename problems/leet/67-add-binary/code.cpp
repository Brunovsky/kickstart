#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string addBinary(string &a, string &b) {
    int A = a.size(), B = b.size(), M = max(A, B);
    if (A < M)
      a = string(M - A, '0') + a;
    if (B < M)
      b = string(M - B, '0') + b;
    string c;
    c.reserve(M + 1);
    int carry = 0;
    for (int i = M - 1; i >= 0; --i) {
      int sum = (a[i] - '0') + (b[i] - '0') + carry;
      carry = (sum & 2) >> 1;
      printf("sum=%d carry=%d a=%c b=%c\n", sum, carry, a[i], b[i]);
      c.push_back((sum & 1) + '0');
    }
    if (carry)
      c.push_back('1');
    reverse(c.begin(), c.end());
    return c;
  }
};

// *****

int main() {
  return 0;
}
