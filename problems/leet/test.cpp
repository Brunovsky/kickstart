#include <bits/stdc++.h>

using namespace std;

void test() {
  const vector<string> tests = {
      "abcd",
      "place",
      "holders",
  };

  const vector<int> expected = {1, 2, 3};

  bool dirty = false;
  for (size_t i = 0; i < tests.size(); ++i) {
    auto actual = Solution{}.call(tests[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %d\n", expected[i]);
    printf("> Actual:   %d\n", actual);
    dirty = true;
  }

  if (!dirty)
    printf("All tests passed!\n");
}
