#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string extend(const string &s) const {
    string E;
    E.reserve(s.size() * 2 + 1);
    E += '/';
    for (size_t i = 0; i < s.size(); ++i) {
      E += s[i];
      E += '/';
    }
    return E;
  }

  string unextend(const string &E) const {
    string s;
    s.reserve(E.size() / 2 + 1);
    for (size_t i = 0; i < E.size(); ++i)
      if (E[i] != '/')
        s += E[i];
    return s;
  }

  // Manacher's with string separator extension
  string longestPalindrome(string s) const {
    if (s.empty())
      return "";

    string E = extend(s);
    int S = E.size();
    int i = 1, C = 0, R = 0;

    vector<int> P(S, 0);
    while (i < S) {
      if (i <= R) {
        P[i] = min(P[2 * C - i], R - i);
      }
      int iL = i - P[i] - 1, iR = i + P[i] + 1;
      while (iL >= 0 && iR < S && E[iR] == E[iL]) {
        ++P[i], ++iR, --iL;
      }
      if (i + P[i] > C + P[C]) {
        C = i;
        R = i + P[i];
      }
      printf("P[%d] = %d\n", i, P[i]);
      assert(P[i] <= i);
      ++i;
    }
    printf("====\n");

    i = 0;
    for (int j = 0; j < S; ++j)
      if (P[i] < P[j])
        i = j;

    return unextend(E.substr(i - P[i], 2 * P[i] + 1));
  }
};

// *****

void test() {
  const vector<string> strings = {
      "babad", "cbbd", "abracadabra", "bananas", "abcbcacbaccbcaa", "abb",
  };
  const vector<string> expected = {
      "bab", "bb", "aca", "anana", "bcacb", "bb",
  };

  // /a/b/b/

  bool dirty = false;
  for (size_t i = 0; i < strings.size(); ++i) {
    auto actual = Solution{}.longestPalindrome(strings[i]);
    if (actual != expected[i]) {
      printf("Error: Actual string did not match expected palindrome.\n");
      printf("Expected: %s\n", expected[i].c_str());
      printf("Actual: %s\n", actual.c_str());
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
