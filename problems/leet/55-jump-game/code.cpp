#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool canJump(const vector<int> &nums) {
    int N = nums.size();
    int can = N - 1;

    for (int i = N - 2; i >= 0; --i) {
      if (can <= i + nums[i])
        can = i;
    }

    return can == 0;
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
