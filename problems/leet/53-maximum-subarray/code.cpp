#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int maxSubArray(const vector<int> &nums) {
    int sum = 0, min_prefix = INT_MAX, max = INT_MIN;

    for (int n : nums) {
      if (min_prefix > sum)
        min_prefix = sum;
      sum += n;
      if (max < sum - min_prefix)
        max = sum - min_prefix;
    }

    return max;
  }
};

// *****

int main() {
  return 0;
}
