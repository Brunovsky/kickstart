#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  void dive(vector<vector<int>> &answers, vector<int> &subset,
            const vector<int> &nums, int k, int n) {
    if (k == n) {
      answers.push_back(subset);
      return;
    }
    dive(answers, subset, nums, k + 1, n);
    subset.push_back(nums[k]);
    dive(answers, subset, nums, k + 1, n);
    subset.pop_back();
  }

public:
  vector<vector<int>> subsets(const vector<int> &nums) {
    vector<vector<int>> answers;
    int n = nums.size();
    answers.reserve(1 << n);
    answers.push_back({});
    for (int i = 0; i < n; ++i) {
      int A = answers.size();
      for (int a = 0; a < A; ++a) {
        vector<int> extended;
        extended.reserve(i + 1);
        extended = answers[a];
        extended.push_back(nums[i]);
        answers.push_back(extended);
      }
    }
    return answers;
  }

  vector<vector<int>> subsets2(vector<int> &nums) {
    vector<vector<int>> answers;
    vector<int> subset;
    int n = nums.size();
    subset.reserve(n);
    dive(answers, subset, nums, 0, n);
    return answers;
  }
};

// *****

int main() {
  return 0;
}
