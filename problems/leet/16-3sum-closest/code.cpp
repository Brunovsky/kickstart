#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int threeSumClosest(vector<int> &nums, int target) {
    sort(nums.begin(), nums.end());
    int N = nums.size();
    int closest = nums[0] + nums[1] + nums[2];

    for (int i = 0; i + 2 < N; ++i) {
      int j = i + 1, k = N - 1;
      while (j < k) {
        int sum = nums[i] + nums[j] + nums[k];
        if (abs(target - closest) > abs(target - sum))
          closest = sum;
        if (sum < target)
          ++j;
        else if (sum > target)
          --k;
        else
          return target;
      }
    }

    return closest;
  }
};

// *****

int main() {
  return 0;
}
