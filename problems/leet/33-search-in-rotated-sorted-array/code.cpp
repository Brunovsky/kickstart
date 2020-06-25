#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int search(vector<int> &nums, int target) {
    int N = nums.size(), l = 0, r = N - 1;
    while (l <= r) {
      int m = (l + r) / 2;
      if (nums[m] == target)
        return m;
      if (nums[l] <= nums[r]) {
        if (target > nums[m])
          l = m + 1;
        else
          r = m - 1;
      } else if (nums[m] < nums[l]) {
        if (target < nums[m] || target >= nums[l])
          r = m - 1;
        else if (target <= nums[r])
          l = m + 1;
        else
          break;
      } else {
        if (target > nums[m] || target <= nums[r])
          l = m + 1;
        else if (target >= nums[l])
          r = m - 1;
        else
          break;
      }
    }
    return -1;
  }
};

// *****

int main() {
  return 0;
}
