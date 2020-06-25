#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> sortArrayByParity(vector<int> &A) {
    int even = 0, a = A.size();

    for (int i = 0; i < a; ++i)
      if ((A[i] % 2) == 0)
        swap(A[i], A[even++]);

    return A;
  }
};

// *****

int main() {
  return 0;
}
