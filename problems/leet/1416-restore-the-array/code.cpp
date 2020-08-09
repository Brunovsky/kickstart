#include <bits/stdc++.h>

using namespace std;

// *****

constexpr long mod = 1e9 + 7;

class Solution {
public:
  int numberOfArrays(string s, int k) {
    int N = s.size();
    vector<long> dp(N + 1, 0);
    dp[N] = 1;

    for (int i = N - 1; i >= 0; i--) {
      if (s[i] == '0') {
        continue;
      }
      int len = 1;
      long n = s[i] - '0';
      while (n <= k && i + len <= N) {
        dp[i] = (dp[i] + dp[i + len]) % mod;
        n = 10 * n + (s[i + len++] - '0');
      }
    }

    return dp[0];
  }
};

// *****

int main() {
  return 0;
}
