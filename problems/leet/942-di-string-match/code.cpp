#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> diStringMatch(string S) {
    int N = S.length();
    int a = 0, b = N;
    vector<int> ans(N + 1);
    for (int i = 0; i < N; i++) {
      if (S[i] == 'I') {
        ans[i] = a++;
      } else {
        ans[i] = b--;
      }
    }
    assert(a == b);
    ans[N] = a;
    return ans;
  }
};

// *****

int main() {
  return 0;
}
