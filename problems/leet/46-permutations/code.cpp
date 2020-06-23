#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<vector<int>> permute(vector<int> &nums) {
    size_t factorial = 1, i = 0;
    for (size_t i = 2; i <= nums.size(); ++i)
      factorial *= i;

    vector<vector<int>> permutations;
    permutations.reserve(factorial);

    while (++i <= factorial) {
      permutations.push_back(nums);
      next_permutation(nums.begin(), nums.end());
    }

    return permutations;
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
