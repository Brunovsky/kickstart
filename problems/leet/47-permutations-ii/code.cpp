#include <bits/stdc++.h>

using namespace std;

// *****

constexpr size_t compute_factorial(size_t n) {
  size_t fact = n;
  while (--n > 1)
    fact *= n;
  return fact;
}

class Solution {
public:
  vector<vector<int>> permuteUnique(vector<int> &nums) {
    size_t i = 0, N = nums.size(), factorial = compute_factorial(N);

    sort(nums.begin(), nums.end());

    while (i < N) {
      size_t j = i + 1;
      while (j < N && nums[i] == nums[j])
        ++j;
      factorial /= compute_factorial(j - i);
      i = j;
    }

    vector<vector<int>> permutations;
    permutations.reserve(factorial);

    i = 0;
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
