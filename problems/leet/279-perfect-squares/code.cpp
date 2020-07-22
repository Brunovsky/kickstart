#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  static vector<int> dp;

  int compute(int n) {
    if (n == 0)
      return 0;
    if (dp.size() > n && dp[n] != 0)
      return dp[n];

    int minimum = n;
    for (int k = floor(sqrt(n)); k > 1; k--) {
      int m = n / (k * k);
      minimum = min(minimum, m + compute(n - m * k * k));
    }

    if (dp.size() <= n)
      dp.resize(n + 1, 0);
    dp[n] = minimum;
    return minimum;
  }

public:
  int numSquares(int n) {
    dp.reserve(n + 1);
    return compute(n);
  }
};

vector<int> Solution::dp;

// *****

int main() {
  return 0;
}
