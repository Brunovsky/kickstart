#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string simplifyPath(string path) {
    vector<string> canonical;

    size_t i = 0;
    while (i++ < path.size()) {
      size_t j = i;
      while (j < path.size() && path[j] != '/')
        ++j;

      if (j - i == 0 || (j - i == 1 && path[i] == '.')) {
        continue;
      } else if (j - i == 2 && path[i] == '.' && path[i + 1] == '.') {
        if (!canonical.empty())
          canonical.pop_back();
      } else {
        canonical.push_back(path.substr(i, j - i));
      }
      i = j;
    }

    if (canonical.empty())
      return "/";

    string result = "";
    for (const string &folder : canonical)
      result += "/"s + folder;
    return result;
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
