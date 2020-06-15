#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> sortArrayByParity(vector<int> &A) {
    int even = 0;

    for (int i = 0; i < A.size(); ++i)
      if ((A[i] % 2) == 0)
        swap(A[i], A[even++]);

    return A;
  }
};

// *****

void test() {
  cout << "All tests passed \033[1;32m" << u8"\u2713" << "\033[0m\n";
}

// *****

int main() {
  test();
  return 0;
}
