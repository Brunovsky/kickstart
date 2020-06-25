#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  void doit(const vector<int> &candidates, const vector<int> &counts,
            vector<vector<int>> &answers, vector<int> &included, int target,
            size_t s) const {
    assert(target >= 0);
    if (target == 0) {
      answers.push_back(included);
      return;
    }
    if (s >= candidates.size())
      return;
    int candidate = candidates[s],
        max_count = min(counts[s], target / candidate);
    for (int count = 1; count <= max_count; ++count) {
      included.push_back(candidate);
      doit(candidates, counts, answers, included, target - count * candidate,
           s + 1);
    }
    included.resize(included.size() - max_count);
    doit(candidates, counts, answers, included, target, s + 1);
  }

  void fill_count(vector<int> &candidates, vector<int> &counts) const {
    size_t s = 0, i = 0, C = candidates.size();
    while (i < C) {
      size_t j = i + 1;
      while (j < C && candidates[i] == candidates[j])
        ++j;
      counts.push_back(j - i);
      candidates[s++] = candidates[i];
      i = j;
    }
    candidates.resize(s);
  }

  vector<vector<int>> combinationSum2(vector<int> &candidates, int target) {
    vector<vector<int>> answers;
    vector<int> included;
    vector<int> counts;
    sort(candidates.begin(), candidates.end());
    fill_count(candidates, counts);
    doit(candidates, counts, answers, included, target, 0);
    assert(included.empty());
    return answers;
  }
};

// *****

int main() {
  return 0;
}
