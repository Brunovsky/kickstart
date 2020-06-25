#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string minWindow(const string &s, const string &t) const {
    if (s.empty() || t.empty() || t.size() > s.size())
      return ""s;

    // how many distinct characters of t are underepresented
    int under = 0;
    // alphabet counts for window and t
    uint16_t window[256] = {}, target[256] = {};
    for (char c : t) {
      ++target[int(c)];
    }
    for (int i = 0; i < 256; ++i) {
      if (target[i]) {
        ++under;
      }
    }

    // window is [i, j), candidate is [best, best+len)
    int S = s.size(), i = 0, j = 0, best = -1, len = INT_MAX;

    // slide
    while (true) {
      // slide j until window is valid
      while (j < S && under) {
        int c = s[j++];
        if (++window[c] == target[c]) {
          --under; // met quota for c
        }
      }
      if (under) {
        break; // window is not valid and j is maxed out
      }

      // trim window from the left by sliding i
      while (true) {
        int c = s[i];
        if (window[c] == target[c]) {
          break;
        }
        --window[c], ++i;
      }

      if (len > j - i)
        best = i, len = j - i;

      // slide i once, making window invalid
      int c = s[i++];
      --window[c], ++under;
    }

    if (best == -1)
      return ""s;
    return s.substr(best, best + len);
  }

  // better solution, one alphabet counter only and implicit window
  string minWindow2(const string &s, const string &t) const {
    if (s.empty() || t.empty() || t.size() > s.size())
      return ""s;

    int window[256] = {};
    for (char c : t) {
      ++window[int(c)];
    }

    int S = s.size(), T = t.size();
    int i = 0;
    int represented = 0;
    int best = 0, len = INT_MAX;

    for (int j = 0; j < S; ++j) {
      int c = s[j];
      if (--window[c] >= 0) {
        ++represented;
      }
      while (represented == T) {
        c = s[i];
        if (window[c]++ >= 0) {
          --represented;
          if (len > j - i + 1) {
            best = i;
            len = j - i + 1;
          }
        }
        ++i;
      }
    }

    if (len == INT_MAX)
      return ""s;
    return s.substr(best, len);
  }
};

// *****

int main() {
  vector<string> s = {"ADOBECODEBANC"s};
  vector<string> t = {"ABC"s};
  vector<string> expected = {"BANC"s};

  for (size_t i = 0; i < s.size(); ++i) {
    auto actual = Solution{}.minWindow(s[i], t[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %s\n", expected[i].c_str());
    printf("> Actual:   %s\n", actual.c_str());
  }

  return 0;
}
