#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  void updatePrefix(string &prefix, const string &str) const {
    size_t i = 0, len = min(prefix.length(), str.length());
    while (i < len && prefix[i] == str[i])
      ++i;
    prefix.resize(i);
  }

  string longestCommonPrefix(const vector<string> &strs) {
    if (strs.empty())
      return "";
    string prefix = strs[0];
    for (size_t i = 1; i < strs.size(); ++i) {
      updatePrefix(prefix, strs[i]);
      if (prefix.empty())
        break;
    }
    return prefix;
  }
};

// *****

int main() {
  return 0;
}
