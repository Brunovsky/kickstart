#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isIsomorphic(string s, string t) {
    char st[128] = {0};
    char ts[128] = {0};
    for (int i = 0, S = s.size(); i < S; i++) {
      // st[cs] == ct  and ts[ct] == cs
      if (st[s[i]] == 0)
        st[s[i]] = t[i];
      if (ts[t[i]] == 0)
        ts[t[i]] = s[i];
      if (st[s[i]] != t[i] || ts[t[i]] != s[i])
        return false;
    }
    return true;
  }
};

// *****

int main() {
  return 0;
}
