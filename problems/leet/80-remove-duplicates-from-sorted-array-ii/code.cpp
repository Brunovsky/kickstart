#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int removeDuplicates(vector<int> &nums) {
    int l = 0, r = 0, N = nums.size();
    while (r < N) {
      int n = nums[r], k = r + 1;
      while (k < N && nums[k] == n)
        ++k;
      for (int i = 0; i < min(2, k - r); ++i)
        nums[l++] = n;
      r = k;
    }
    return l;
  }
};

// *****

int main() {
  return 0;
}
