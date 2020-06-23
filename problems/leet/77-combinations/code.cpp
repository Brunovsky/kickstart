#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  void dive(vector<vector<int>> &answers, vector<int> &included, int n, int m,
            int k) const {
    assert(m + k <= n + 1);
    if (k == 0) {
      answers.push_back(included);
      return;
    }
    for (int i = m; i + k <= n + 1; ++i) {
      included.push_back(i);
      dive(answers, included, n, i + 1, k - 1);
      included.pop_back();
    }
  }

public:
  vector<vector<int>> combine(int n, int k) {
    vector<vector<int>> answers;
    vector<int> included;
    dive(answers, included, n, 1, k);
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
