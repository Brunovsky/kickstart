#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int RED = 0, WHITE = 1, BLUE = 2;

class Solution {
public:
  void sortColors(vector<int> &nums) {
    for (int red = 0, white = 0, blue = 0, N = nums.size(); blue < N;) {
      if (nums[blue] == BLUE)
        blue++;
      else if (nums[blue] == WHITE)
        nums[blue++] = BLUE, nums[white++] = WHITE;
      else /* nums[blue] == RED */
        nums[blue++] = BLUE, nums[white++] = WHITE, nums[red++] = RED;
    }
  }
};

// *****

int main() {
  return 0;
}
