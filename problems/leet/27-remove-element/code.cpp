#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int removeElement(vector<int> &nums, int val) {
    int i = 0;
    for (size_t j = 0; j < nums.size(); ++j)
      if (nums[j] != val)
        nums[i++] = nums[j];
    return i;
  }
};

// *****

int main() {
  return 0;
}
