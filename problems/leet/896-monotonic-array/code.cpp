#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  bool isMonotonic(vector<int> &A) {
    if (A.size() <= 1)
      return true;
    int a = A.size();
    int direction = A.back() - A.front();
    if (direction <= 0)
      for (int i = 0; i + 1 < a; ++i)
        if (A[i] < A[i + 1])
          return false;
    if (direction >= 0)
      for (int i = 0; i + 1 < a; ++i)
        if (A[i] > A[i + 1])
          return false;
    return true;
  }
};

// *****

int main() {
  return 0;
}
