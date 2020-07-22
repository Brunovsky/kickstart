#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int findMaxForm(vector<string> &strs, int M, int N) {
    int S = strs.size();
    vector<array<int, 2>> cnt(S);

    for (int i = 0; i < S; i++) {
      for (char c : strs[i]) {
        cnt[i][c - '0']++;
      }
    }

    sort(begin(cnt), end(cnt));
    int zeros = 0;
    int ones = 0;
    int accepted = 0;
    multiset<int, greater<int>> oneset;

    for (int i = 0; i < S; i++) {
      if (zeros + cnt[i][0] > M) {
        return accepted;
      } else if (ones + cnt[i][1] <= N) {
        // zeros + cnt[i][0] <= M
        zeros += cnt[i][0];
        ones += cnt[i][1];
        accepted++;
        oneset.insert(cnt[i][1]);
      } else if (!oneset.empty()) {
        // ones + cnt[i][1] > N
        int ones_max = *oneset.begin();
        if (ones_max > cnt[i][1]) {
          ones -= ones_max - cnt[i][1];
          oneset.erase(oneset.begin());
        }
      }
    }

    return accepted;
  }
};

// *****

int main() {
  return 0;
}
