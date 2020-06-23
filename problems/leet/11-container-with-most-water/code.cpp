#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int maxArea(const vector<int> &height) {
    int H = height.size();
    int L = 0, R = H - 1;

    int water = 0;
    while (L < R) {
      int min_height = min(height[L], height[R]);
      water = max(water, min_height * (R - L));

      while (L < R && height[L] <= min_height)
        ++L;
      while (L < R && height[R] <= min_height)
        --R;
    }

    return water;
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
