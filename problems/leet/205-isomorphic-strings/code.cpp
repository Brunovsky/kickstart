#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isIsomorphic(string s, string t) {
    char st[128] = {0};
    char ts[128] = {0};
    for (int i = 0, S = s.size(); i < S; i++) {
      char &smap = st[int(s[i])];
      char &tmap = ts[int(t[i])];
      if (smap == 0)
        smap = t[i];
      if (tmap == 0)
        tmap = s[i];
      if (smap != t[i] || tmap != s[i])
        return false;
    }
    return true;
  }
};

// *****

int main() {
  return 0;
}
