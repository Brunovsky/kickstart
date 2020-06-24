#include <bits/stdc++.h>

using namespace std;

// *****

// No one's got the fucking patience...
auto flags = std::regex_constants::ECMAScript | std::regex_constants::optimize |
             std::regex_constants::nosubs;

// in exponent, allow e+9 and e-9 but disallow decimals
// allow 'e' but not 'E'
// allow leading and trailing spaces but none in the middle
// allow .25 and the like (allowed by leet's test cases but undocumented)
// allow no digits after decimal point (wtf?)
// allow optional leading +-
std::regex num_regex(R"_(\s*[+-]?(\d+\.?|\d*\.\d+)(e[+-]?\d+)?\s*)_", flags);

class Solution {
public:
  bool isNumber(string s) {
    return std::regex_match(s, num_regex);
  }
};

// *****

void test() {
  map<string, bool> tests = {
      {"0", true},     {" 0.1 ", true},     {"abc", false},    {"1 a", false},
      {"2e10", true},  {" -90e3   ", true}, {" 1e", false},    {"e3", false},
      {" 6e-1", true}, {" 99e2.5 ", false}, {"53.5e93", true}, {" --6 ", false},
      {"-+3", false},  {"95a54e53", false}, {".25", true},     {".1", true},
      {".30e9", true}, {"3.", true},        {"25.e3", true},
  };

  bool dirty = false;
  for (const auto [s, expected] : tests) {
    auto actual = Solution{}.isNumber(s);
    if (actual != expected) {
      printf("Test '%s' failed\n", s.c_str());
      printf("> Expected: %s\n", expected ? "true" : "false");
      printf("> Actual:   %s\n", actual ? "true" : "false");
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
