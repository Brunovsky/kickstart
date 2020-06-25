#include <bits/stdc++.h>

using namespace std;

// *****

// The three great qualities of programmers: laziness, impatience, and hubris.

class Solution {
public:
  vector<int> searchRange(vector<int> &nums, int target) {
    int lower = lower_bound(nums.begin(), nums.end(), target) - nums.begin();
    int upper = upper_bound(nums.begin(), nums.end(), target) - nums.begin();
    int S = nums.size();
    if (lower == S || nums[lower] != target)
      return {-1, -1};
    else
      return {lower, upper - 1};
  }
};

// *****

int main() {
  return 0;
}
