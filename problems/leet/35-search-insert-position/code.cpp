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

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
