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

int main() {
  return 0;
}
