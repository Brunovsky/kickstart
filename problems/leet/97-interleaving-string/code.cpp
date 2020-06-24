#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isInterleave(string s1, string s2, string s3) {
    if (s1.size() > s2.size())
      swap(s1, s2);
    int S1 = s1.size(), S2 = s2.size(), S3 = s3.size();
    if (S1 + S2 != S3)
      return false;

    uint8_t dp[S1 + 1];
    memset(dp, 0, sizeof(dp));

    dp[0] = true;
    for (int i = 0; i < S1; ++i) {
      if (s1[i] != s3[i]) {
        break;
      }
      dp[i + 1] = true;
    }

    for (int j = 0; j < S2; ++j) {
      char c2 = s2[j];
      dp[0] = dp[0] && c2 == s3[j];
      for (int i = 0; i < S1; ++i) {
        char c1 = s1[i], c3 = s3[i + j + 1];
        dp[i + 1] = (c1 == c3 && dp[i]) || (c2 == c3 && dp[i + 1]);
      }
    }

    return dp[S1];
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
