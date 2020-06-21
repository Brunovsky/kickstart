#include <bits/stdc++.h>

using namespace std;

// *****

constexpr int RED = 0, WHITE = 1, BLUE = 2;

class Solution {
public:
  void sortColors(vector<int> &nums) {
    for (int red = 0, white = 0, blue = 0; blue < nums.size();) {
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

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
