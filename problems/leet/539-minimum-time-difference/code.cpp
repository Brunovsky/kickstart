#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  inline int hour(const string &s) const {
    return 10 * (s[0] - '0') + (s[1] - '0');
  }
  inline int mins(const string &s) const {
    return 10 * (s[3] - '0') + (s[4] - '0');
  }

public:
  int findMinDifference(const vector<string> &time) {
    vector<int> minutes(time.size());
    int T = time.size();
    for (int i = 0; i < T; ++i) {
      minutes[i] = hour(time[i]) * 60 + mins(time[i]);
    }
    sort(minutes.begin(), minutes.end());
    int best = 1440;
    for (int i = 1; i < T; ++i) {
      best = std::min(best, minutes[i] - minutes[i - 1]);
    }
    best = std::min(best, 1440 - (minutes.back() - minutes.front()));
    return best;
  }
};

// *****

int main() {
  return 0;
}
