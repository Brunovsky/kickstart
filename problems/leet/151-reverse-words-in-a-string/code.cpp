#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  string reverseWords(string s) {
    reverse(s.begin(), s.end());
    int S = s.size();
    int i = 0;
    while (i < S) {
      while (i < S && s[i] == ' ')
        ++i;
      if (i == S)
        break;
      int j = i + 1;
      while (j < S && s[j] != ' ')
        ++j;
      reverse(s.begin() + i, s.begin() + j);
      i = j;
    }
    int read = 0, write = 0;
    while (read < S) {
      while (read < S && s[read] == ' ')
        read++;
      if (read == S)
        break;
      if (write > 0)
        s[write++] = ' ';
      while (read < S && s[read] != ' ')
        s[write++] = s[read++];
    }
    s.resize(write);
    return s;
  }
};

// *****

int main() {
  return 0;
}
