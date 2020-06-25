#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<bool> canMakePaliQueries(string s, vector<vector<int>> &queries) {
    int S = s.size(), Q = queries.size();
    if (Q == 0)
      return {};

    vector<int> mask(S + 1);
    mask[0] = 0;
    for (int i = 1; i <= S; ++i)
      mask[i] = mask[i - 1] ^ (1 << (s[i - 1] - 'a'));

    constexpr int LEFT = 0, RIGHT = 1, K = 2;

    vector<bool> answer;
    answer.reserve(Q);
    for (const auto &q : queries) {
      int odds = __builtin_popcount(mask[q[RIGHT] + 1] ^ mask[q[LEFT]]);
      answer.push_back(odds <= 2 * q[K] + 1);
    }
    return answer;
  }
};

// *****

int main() {
  return 0;
}
