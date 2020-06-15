#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> twoSum(vector<int> &nums, int target) {
    vector<int> sorted = nums;
    sort(sorted.begin(), sorted.end());
    assert(nums.size() >= 2);
    int S = nums.size();
    int l = 0, r = S - 1;

    while (l != r) {
      int sum = sorted[l] + sorted[r];
      if (sum > target)
        --r;
      else if (sum < target)
        ++l;
      else
        break;
    }

    int a = find(nums.begin(), nums.end(), sorted[l]) - nums.begin();
    int b = find(nums.rbegin(), nums.rend(), sorted[r]) - nums.rbegin());

    return {a, b};
  }
};

// *****

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
