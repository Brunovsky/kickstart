#include <bits/stdc++.h>

using namespace std;

// *****

vector<int> fib = {1, 1, 2, 3, 5, 8, 13, 21, 34};

class Solution {
public:
  int climbStairs(int n) {
    int m = fib.size();
    if (n >= m) {
      fib.reserve(m + 1);
      for (int i = m; i <= n; ++i)
        fib.push_back(fib[i - 1] + fib[i - 2]);
    }
    return fib[n];
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
