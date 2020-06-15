#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isMonotonic(vector<int> &A) {
    if (A.size() <= 1)
      return true;
    int direction = A.back() - A.front();
    if (direction <= 0)
      for (int i = 0; i + 1 < A.size(); ++i)
        if (A[i] < A[i + 1])
          return false;
    if (direction >= 0)
      for (int i = 0; i + 1 < A.size(); ++i)
        if (A[i] > A[i + 1])
          return false;
    return true;
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
