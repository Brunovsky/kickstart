#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int subarraysWithKDistinct(const vector<int> &A, int K) {
    int N = A.size();
    vector<int> cntl(N + 1, 0);
    vector<int> cntr(N + 1, 0);

    int l = 0, r = 0;
    int posl = 0, posr = 0;
    int ans = 0;

    for (int i = 0; i < N; i++) {
      posl += !cntl[A[i]]++;
      posr += !cntr[A[i]]++;

      while (posl == K + 1) {
        posl -= !--cntl[A[l++]];
      }
      while (posr == K) {
        posr -= !--cntr[A[r++]];
      }

      ans += r - l;
    }

    return ans;
  }
};

// *****

Solution sol;

int main() {
  cout << sol.subarraysWithKDistinct({1, 2, 1, 3, 4}, 3) << '\n';
  cout << sol.subarraysWithKDistinct({1, 2, 1, 2, 3}, 2) << '\n';
  return 0;
}
