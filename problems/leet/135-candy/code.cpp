#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int candy(const vector<int> &ratings) {
    int up = 1, down = 1, peak = 1;
    int total = 1, N = ratings.size();
    if (N == 0)
      return 0;

    for (int i = 1; i < N; ++i) {
      if (ratings[i - 1] < ratings[i])
        total += ++up, peak = up, down = 1;
      else if (ratings[i - 1] > ratings[i])
        total += (down + (down >= peak)), ++down, up = 1;

      // make sure you don't misread the problem.
      // this case is meant to reset everything.
      else if (ratings[i - 1] == ratings[i])
        total += 1, up = down = peak = 1;
    }

    return total;
  }
};

// *****

/**
 * *   *            *              *
 *   *            *            * *
 *              *
 * ----------------------------------
 * *
 *   * *          * *          * * *
 *              *
 * ----------------------------------
 * *
 *   *            *            * *
 *     *        *   *              *
 */

/**
 * * 1: (up==1,down>=2,peak>=1) -> ++up -> (up+1,down=1,peak=up)
 * * 2: (up>=2,down==1,peak>=2) -> ++up -> (up+1,down=1,peak=up)
 * * 3: --
 *
 * * 1: --
 * * 2: --
 * * 3: --
 *
 * * 1: (up==1,down>=2,peak>=1) -> down++ (+1 if down>=peak) ->
 * * 2: (up>=2,down==1,peak>=2) -> down++ -> (up=1,down+1,peak)
 * * 3: --
 */

int main() {
  const vector<vector<int>> tests = {
      {1, 0, 2},
      {1, 2, 2},
      {1, 3, 2, 2, 1},
  };

  const vector<int> expected = {5, 4, 7};

  for (size_t i = 0; i < tests.size(); ++i) {
    int actual = Solution{}.candy(tests[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %d\n", expected[i]);
    printf("> Actual:   %d\n", actual);
  }

  return 0;
}
