#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int maxProfit(const vector<int> &prices) {
    int P = prices.size();
    if (P == 0)
      return 0;

    vector<int> left(P);
    vector<int> right(P);

    left[0] = prices[0];
    right[P - 1] = prices[P - 1];

    for (int i = 1; i < P; ++i)
      left[i] = min(left[i - 1], prices[i]);

    for (int i = P - 2; i >= 0; --i)
      right[i] = max(right[i + 1], prices[i]);

    left[0] = right[P - 1] = 0;

    for (int i = 1; i < P; ++i)
      left[i] = max(left[i - 1], prices[i] - left[i]);

    for (int i = P - 2; i >= 0; --i)
      right[i] = max(right[i + 1], right[i] - prices[i]);

    int best = 0;

    for (int i = 0; i <= P; ++i) {
      int best_left = (i > 0) ? left[i - 1] : 0;
      int best_right = (i < P) ? right[i] : 0;
      if (best < best_left + best_right)
        best = best_left + best_right;
    }

    return best;
  }
};

// *****

void test() {
  const vector<vector<int>> tests = {
      {3, 3, 5, 0, 0, 3, 1, 4},
      {1, 2, 3, 4, 5},
      {7, 6, 4, 3, 1},
      {3, 2, 6, 5, 0, 3},
  };

  vector<int> expected = {6, 4, 0, 7};

  bool dirty = false;
  for (size_t i = 0; i < tests.size(); ++i) {
    int profit = Solution{}.maxProfit(tests[i]);
    if (profit != expected[i]) {
      printf("Test #%lu failed\n", i);
      printf("> Expected: %d\n", expected[i]);
      printf("> Actual:   %d\n", profit);
      dirty = true;
    }
  }

  if (!dirty)
    cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
