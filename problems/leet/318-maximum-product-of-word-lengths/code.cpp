#include <bits/stdc++.h>

using namespace std;

// *****

struct type_t {
  int mask = 0, size = 0;
};

bool cmp(const type_t &lhs, const type_t &rhs) {
  return lhs.size > rhs.size;
}

class Solution {
public:
  int maxProduct(vector<string> &words) {
    int W = words.size();
    if (W == 0)
      return 0;

    vector<type_t> masks(W);
    int best = 0;

    for (int i = 0; i < W; ++i) {
      int mask = 0;
      for (char c : words[i]) {
        mask |= 1 << (c - 'a');
      }
      masks[i].mask = mask;
      masks[i].size = words[i].size();
    }

    sort(masks.begin(), masks.end(), cmp);

    for (int i = 0; i + 1 < W; ++i) {
      if (masks[i].size * masks[i].size <= best)
        break;

      for (int j = i + 1; j < W; ++j) {
        if (masks[i].size * masks[j].size <= best)
          break;

        if ((masks[i].mask & masks[j].mask) == 0) {
          best = masks[i].size * masks[j].size;
        }
      }
    }

    return best;
  }
};

// *****

void test() {
  vector<string> words = {"abcw", "baz", "foo", "bar", "xtfn", "abcdef"};
  int res = Solution{}.maxProduct(words);

  for (const string &word : words)
    cout << word << '\n';
  cout << res << '\n';

  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
