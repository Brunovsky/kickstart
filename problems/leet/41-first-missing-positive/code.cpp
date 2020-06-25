#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int firstMissingPositive(vector<int> &nums) {
    int size = nums.size();

    for (int i = 0; i < size; ++i) {
      int prev = ~nums[i];
      while (nums[i] != i + 1 && nums[i] > 0 && nums[i] <= size &&
             nums[i] != prev) {
        prev = nums[i];
        swap(nums[i], nums[nums[i] - 1]);
      }
    }

    for (int i = 0; i < size; ++i) {
      if (nums[i] != i + 1)
        return i + 1;
    }

    return size + 1;
  }
};

// *****

int main() {
  vector<vector<int>> tests = {
      {1, 2, 0}, {3, 4, -1, 1},         {7, 8, 9, 11, 12},  {1},
      {2},       {4, 6, 2, 7, 1, 5, 3}, {2, 4, 3, 7, 6, 3},
  };

  const vector<int> expected = {3, 2, 1, 2, 1, 8, 1};

  for (size_t i = 0; i < tests.size(); ++i) {
    auto actual = Solution{}.firstMissingPositive(tests[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %d\n", expected[i]);
    printf("> Actual:   %d\n", actual);
  }

  return 0;
}
