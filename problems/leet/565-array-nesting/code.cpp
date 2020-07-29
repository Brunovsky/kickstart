#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int arrayNesting(vector<int> &A) {
    int N = A.size();
    vector<bool> vis(N, false);

    int longest = 0;
    for (int i = 0; i < N; i++) {
      if (vis[A[i]]) {
        continue;
      }
      int cycle = 0;
      int a = A[i];
      do {
        vis[a] = true;
        a = A[a];
        cycle++;
      } while (a != A[i]);
      longest = max(cycle, longest);
    }
    return longest;
  }
};

// *****

int main() {
  return 0;
}
