#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool wordBreak(const string &s, const vector<string> &wordDict) {
    int S = s.size();
    vector<bool> v(S, false);
    vector<int> indices;
    indices.push_back(0);
    v[0] = true;

    while (!indices.empty()) {
      int i = indices.back();
      indices.pop_back();
      v[i] = true;

      for (const auto &word : wordDict) {
        int w = word.size();
        if (word == s.substr(i, w)) {
          if (v[i + w])
            continue;
          if (i + w == S)
            return true;
          indices.push_back(i + w);
          v[i + w] = true;
        }
      }
    }

    return false;
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
