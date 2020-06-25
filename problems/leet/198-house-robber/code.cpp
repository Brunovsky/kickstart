#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int rob(const vector<int> &nums) {
    size_t N = nums.size();
    if (N == 0)
      return 0;

    int robbed = nums[0], skipped = 0;

    for (size_t i = 1; i < N; ++i) {
      int new_robbed = skipped + nums[i];
      int new_skipped = max(robbed, skipped);
      robbed = new_robbed, skipped = new_skipped;
    }

    return max(robbed, skipped);
  }
};

// *****

int main() {
  return 0;
}
