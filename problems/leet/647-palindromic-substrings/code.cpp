#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int countSubstrings(string s) {
    string t;
    t += '\0';
    for (char c : s) {
      t += c, t += '\0';
    }

    int T = t.size();
    int c = 0, r = 0;
    vector<int> P(T, 0);

    for (int i = 0; i < T; i++) {
      if (i < c + P[c]) {
        P[i] = min(c + P[c] - i, P[2 * c - i]);
      }
      int L = i - (P[i] + 1);
      int R = i + (P[i] + 1);
      while (L >= 0 && R < T && t[L] == t[R]) {
        L--, R++, P[i]++;
      }
      if (i + P[i] > c + P[c]) {
        c = i;
      }
    }

    int cnt = 0;
    for (int i = 0; i < T; i++) {
      cnt += (P[i] + 1) / 2;
    }
    return cnt;
  }
};

// *****

int main() {
  return 0;
}
