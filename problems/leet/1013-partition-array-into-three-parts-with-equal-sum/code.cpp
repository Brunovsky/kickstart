#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool canThreePartsEqualSum(vector<int> &A) {
    int sum = accumulate(begin(A), end(A), 0);
    if (sum % 3)
      return false;
    int each = sum / 3;
    int i = 0, n = A.size();
    sum = 0;
    while (i < n) {
      sum += A[i++];
      if (sum == each)
        break;
    }
    while (i < n) {
      sum += A[i++];
      if (sum == 2 * each)
        break;
    }
    return i < n;
  }
};

// *****

int main() {
  return 0;
}
