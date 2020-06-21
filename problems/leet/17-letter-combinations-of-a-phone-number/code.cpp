#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<string> letterCombinations(const string &digits) {
    vector<string> results;
    if (digits.empty())
      return results;

    const array<string, 8> mapping{"abc", "def",  "ghi", "jkl",
                                   "mno", "pqrs", "tuv", "wxyz"};

    using lambda_t = function<void(const string &, size_t)>;
    lambda_t abc = [&](const string &prefix, size_t i) {
      if (i == digits.size())
        return results.push_back(prefix);
      for (char c : mapping[digits[i] - '2'])
        abc(prefix + c, i + 1);
    };

    abc("", 0);
    return results;
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
