#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<string> fullJustify(vector<string> &words, int maxWidth) {
    vector<string> lines;

    int i = 0, j = 0, w = 0, s = 0, W = words.size();
    while (j < W) {
      w = words[j++].length();
      s = 0;
      while (j < W && w + s + int(words[j].length()) + 1 <= maxWidth)
        w += words[j++].length(), ++s;

      string line = words[i++];

      if (j < W) {
        if (s == 0) {
          line.append(maxWidth - w, ' ');
        } else {
          int Sd = (maxWidth - w) / s, Sm = (maxWidth - w) % s;

          string space = string(Sd, ' ');
          while (i < j)
            line.append(Sd + (Sm-- > 0), ' ').append(words[i++]);
        }
      } else {
        while (i < j)
          line.append(1, ' ').append(words[i++]);
        line.append(maxWidth - w - s, ' ');
      }

      lines.push_back(move(line));
    }
    return lines;
  }
};

// *****

int main() {
  return 0;
}
