#include <bits/stdc++.h>

using namespace std;

// *****

constexpr char ANY = '.', MANY = '*';
#define MATCH(s, p) ((*s == *p || *p == ANY) && *s != 0)

class Solution {
  // avoid substring and indexing fiesta
  bool match(const char *s, const char *p) const {
    if (!*s && !*p) {
      return true;
    }
    if (!*p) {
      return false;
    }
    if (*(p + 1) == MANY) {
      return (MATCH(s, p) && match(s + 1, p)) || match(s, p + 2);
    }
    if (MATCH(s, p)) {
      return match(s + 1, p + 1);
    }
    return false;
  }

public:
  bool isMatch(const string &s, const string &p) const {
    return match(s.c_str(), p.c_str());
  }
};

// *****

int main() {
  const vector<string> s = {
      "aa", "aa", "ab", "aab", "mississipi", "aaa", "aaa", "a", "a", "ab",
  };

  const vector<string> p = {
      "a",       "a*",       "ab",  "c*a*b",  "mis*is*p*.",
      "ab*ac*a", "ab*a*c*a", "ab*", ".*..a*", ".*..",
  };

  const vector<bool> expected = {
      false, true, true, true, false, true, true, true, false, true,
  };

  for (size_t i = 0; i < s.size(); ++i) {
    auto actual = Solution{}.isMatch(s[i], p[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("'%s'  matches  '%s'\n", s[i].c_str(), p[i].c_str());
    printf("> Expected: %s\n", expected[i] ? "true" : "false");
    printf("> Actual:   %s\n", actual ? "true" : "false");
  }

  return 0;
}
