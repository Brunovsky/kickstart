#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int maxSumDivThree(vector<int> &nums) {
    constexpr int inf = 1e7;
    int min[2][2] = {{inf, inf}, {inf, inf}};
    int cnt = 0;
    int sum = 0;
    for (int n : nums) {
      int c = (n + 2) % 3;
      sum += n;
      if (c != 2) {
        cnt += c ? 4 : 2;
        if (n < min[c][1]) {
          min[c][1] = n;
          if (min[c][0] > min[c][1]) {
            swap(min[c][0], min[c][1]);
          }
        }
      }
    }
    int c = (cnt + 2) % 3;
    if (c == 2) {
      return sum;
    } else {
      return sum - std::min(min[!c][0], min[c][0] + min[c][1]);
    }
  }
};

// *****

int main() {
  return 0;
}
