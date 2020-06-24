#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
  void dive(vector<string> &answers, string pprefix, string s, //
            int sets, int i) {
    if (sets == 4) {
      if (i == s.size()) {
        answers.push_back(pprefix);
      }
      return;
    }
    if (i >= s.size()) {
      return;
    }
    string prefix = pprefix;
    if (sets)
      prefix += '.';
    for (int j = 1; j <= 3 && i + j <= s.size(); ++j) {
      string part = s.substr(i, j);
      if (j == 3 && part > "255"s)
        break;
      dive(answers, prefix + part, s, sets + 1, i + j);
      if (j == 1 && part == "0"s)
        break;
    }
  }

public:
  vector<string> restoreIpAddresses(string s) {
    vector<string> answers;
    dive(answers, "", s, 0, 0);
    return answers;
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
