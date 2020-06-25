#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int searchInsert(vector<int> &nums, int target) {
    return lower_bound(nums.begin(), nums.end(), target) - nums.begin();
  }
};

// *****

int main() {
  return 0;
}
