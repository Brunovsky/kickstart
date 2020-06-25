#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool search(vector<int> &nums, int target) {
    int N = nums.size(), l = 0, r = N - 1;
    while (l < r && nums[l] == nums[r])
      --r;
    while (l <= r) {
      int m = (l + r) / 2;
      if (nums[m] == target)
        return true;
      if (nums[l] == nums[m] && nums[m] == nums[r]) {
        int L = l + 1, R = r - 1;
        while (L <= R && nums[L] == nums[m])
          ++L;
        while (L <= R && nums[m] == nums[R])
          --R;
        l = L, r = R;
      }
      // by the previous check, this means [l,m] or [m,r] is sorted
      else if (nums[l] <= nums[m]) {
        if (nums[l] <= target && target < nums[m])
          r = m - 1;
        else
          l = m + 1;
      }
      // by the previous check, this means [l,m] is unsorted, so [m,r] is sorted
      else /* nums[l] > nums[m] */ {
        if (nums[l] <= target || target < nums[m])
          r = m - 1;
        else
          l = m + 1;
      }
    }
    return false;
  }
};

// *****

int main() {
  return 0;
}
