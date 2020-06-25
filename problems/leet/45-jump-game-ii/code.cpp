#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int jump(const vector<int> &nums) {
    if (nums.size() <= 1ul)
      return 0;
    int left = 0, right = 1, last_index = nums.size() - 1, jumps = 0;

    for (int i = 1; i < last_index; ++i) {
      if (nums[i] - nums[left] + i >= nums[right] - nums[left] + right) {
        right = i;
      }
      if (i - left == nums[left]) {
        left = right;
        right = left + 1;
        ++jumps;
      }
    }

    if (left != last_index)
      ++jumps;
    return jumps;
  }
};

// *****

int main() {
  return 0;
}
