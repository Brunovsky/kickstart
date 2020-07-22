#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int hIndex(vector<int> &citations) {
    int N = citations.size();
    int L = 0, R = N - 1;
    while (L <= R) {
      int M = (L + R) / 2;
      if (N - M <= citations[M]) {
        R = M - 1;
      } else {
        L = M + 1;
      }
    }
    return N - L;
  }
};

// *****

int main() {
  return 0;
}
