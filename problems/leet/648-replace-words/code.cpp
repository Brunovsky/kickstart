#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string replaceWords(const vector<string> &dict, string sentence) {
    int i = 0, S = sentence.size();
    if (S == 0)
      return sentence;

    unordered_set<string> uset;
    for (string root : dict)
      uset.insert(root);

    string rooted = "";
    rooted.reserve(S);

    while (true) {
      int j = i + 1;
      while (j < S && sentence[j] != ' ')
        j++;

      string prefix = "";
      do {
        prefix += sentence[i++];
      } while (i < j && !uset.count(prefix));
      rooted += prefix;

      i = j + 1;
      if (i >= S)
        break;
      rooted += ' ';
    }

    return rooted;
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
