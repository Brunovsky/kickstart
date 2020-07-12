#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int rob(const vector<int> &nums) {
    int N = nums.size();
    int best = 0;
    int robbed = 0;
    int skipped = 0;

    if (N == 1)
      return nums[0];

    // case 1: we rob the first house
    for (int i = 0; i + 1 < N; i++) {
      int new_robbed = skipped + nums[i];
      skipped = max(robbed, skipped);
      robbed = new_robbed;
    }
    best = max(robbed, skipped);

    // case 2: we rob the last house
    robbed = skipped = 0;
    for (int i = 1; i < N; i++) {
      int new_robbed = skipped + nums[i];
      skipped = max(robbed, skipped);
      robbed = new_robbed;
    }
    best = max(best, max(robbed, skipped));

    return best;
  }
};

// *****

int main() {
  return 0;
}
