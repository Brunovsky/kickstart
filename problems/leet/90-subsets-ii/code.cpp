#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<vector<int>> subsetsWithDup(vector<int> &nums) {
    sort(nums.begin(), nums.end());
    vector<vector<int>> answers;
    answers.push_back({});

    int i = 0, N = nums.size();
    while (i < N) {
      int num = nums[i];
      int j = i + 1;
      while (j < N && num == nums[j])
        ++j;
      int count = j - i;

      int A = answers.size();
      for (int a = 0; a < A; ++a) {
        vector<int> extended;
        extended.reserve(answers[a].size() + count);
        extended = answers[a];
        for (int c = 1; c <= count; ++c) {
          extended.push_back(num);
          answers.push_back(extended);
        }
      }

      i = j;
    }

    return answers;
  }
};

// *****

int main() {
  return 0;
}
