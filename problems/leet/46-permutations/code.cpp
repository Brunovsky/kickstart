#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<vector<int>> permute(vector<int> &nums) {
    size_t factorial = 1;
    for (size_t i = 2; i <= nums.size(); ++i)
      factorial *= i;

    vector<vector<int>> permutations;
    permutations.reserve(factorial);

    size_t i = 0;
    while (++i <= factorial) {
      permutations.push_back(nums);
      next_permutation(nums.begin(), nums.end());
    }

    return permutations;
  }
};

// *****

int main() {
  return 0;
}
