#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<vector<int>> threeSum(vector<int> &nums) {
    int S = nums.size();
    if (S < 3)
      return {};

    sort(nums.begin(), nums.end());

    vector<vector<int>> solution;

    for (int h = 0; h < S - 2; ++h) {
      int l = h + 1, r = S - 1;

      if (h > 0 && nums[h] == nums[h - 1])
        continue;
      if (nums[h] + nums[h + 1] + nums[h + 2] > 0)
        break;

      while (l < r) {
        int sum = nums[h] + nums[l] + nums[r];
        if (sum < 0)
          ++l;
        if (sum > 0)
          --r;
        if (sum == 0) {
          solution.push_back({nums[h], nums[l++], nums[r--]});
          while (l < r && nums[l] == nums[l - 1])
            ++l;
          while (l < r && nums[r] == nums[r + 1])
            --r;
        }
      }
    }

    return solution;
  }
};

// *****

void test() {
  Solution S;

  vector<int> nums1{-1, 0, 1, 2, -1, -4};

  auto sol = S.threeSum(nums1);

  for (auto triple : sol) {
    cout << triple[0] << ',' << triple[1] << ',' << triple[2] << endl;
  }
}

// *****

int main() {
  test();
  return 0;
}
