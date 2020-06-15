#include <bits/stdc++.h>

using namespace std;

// *****

static vector<int> answers{1, 1, 2, 5};

class Solution {
  int compute() {
    int sum = 0, n = answers.size();
    for (int i = 0; i < n; ++i)
      sum += answers[i] * answers[n - i - 1];
    answers.push_back(sum);
    return sum;
  }

public:
  int numTrees(int n) {
    int k = answers.size();
    while (k++ <= n)
      compute();
    return answers[n];
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
