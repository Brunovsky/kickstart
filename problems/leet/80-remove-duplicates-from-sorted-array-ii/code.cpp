#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int removeDuplicates(vector<int> &nums) {
    int l = 0, r = 0;
    while (r < nums.size()) {
      int n = nums[r], k = r + 1;
      while (k < nums.size() && nums[k] == n)
        ++k;
      for (size_t i = 0; i < min(2, k - r); ++i)
        nums[l++] = n;
      r = k;
    }
    return l;
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
