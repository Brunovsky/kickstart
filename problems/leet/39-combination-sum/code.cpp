#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  void doit(const vector<int> &candidates, vector<vector<int>> &answers,
            vector<int> &included, int target, size_t i) const {
    assert(target >= 0);
    if (target == 0) {
      answers.push_back(included);
      return;
    }
    if (i >= candidates.size())
      return;
    int count = 1, candidate = candidates[i];
    while (count * candidate <= target) {
      included.push_back(candidate);
      doit(candidates, answers, included, target - count++ * candidate, i + 1);
    }
    included.resize(included.size() - (count - 1));
    doit(candidates, answers, included, target, i + 1);
  }

  vector<vector<int>> combinationSum(vector<int> &candidates, int target) {
    vector<vector<int>> answers;
    vector<int> included;
    doit(candidates, answers, included, target, 0);
    assert(included.empty());
    return answers;
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
