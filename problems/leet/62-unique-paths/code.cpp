#include <bits/stdc++.h>

using namespace std;

// *****

// The implementation of this calculation could theoretically overflow for some
// input values. For LeetCode's inputs it does not.
int binomial(uint64_t n, uint64_t k) {
  uint64_t product = 1, d = 1;
  k = min(k, n - k);
  while (d <= k) {
    product *= n--;
    product /= d++;
  }
  return int(product);
}

class Solution {
public:
  int uniquePaths(int m, int n) {
    return binomial(m + n - 2, m - 1);
  }
};

// *****

int main() {
  return 0;
}
