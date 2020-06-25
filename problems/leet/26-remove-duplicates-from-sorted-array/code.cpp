#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int removeDuplicates(vector<int> &nums) {
    if (nums.empty())
      return 0;
    int i = 0;
    for (size_t j = 1; j < nums.size(); ++j)
      if (nums[i] != nums[j])
        nums[++i] = nums[j];
    return i + 1;
  }
};

// *****

int main() {
  return 0;
}
