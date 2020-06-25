#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int trap(const vector<int> &height) {
    if (height.size() <= 2)
      return 0;

    size_t imax = max_element(height.begin(), height.end()) - height.begin();

    int trapped = 0, max;

    max = height.front();
    for (size_t i = 1; i < imax; ++i) {
      if (height[i] >= max) {
        max = height[i];
      } else {
        trapped += max - height[i];
      }
    }

    max = height.back();
    for (size_t i = height.size() - 2; i > imax; --i) {
      if (height[i] >= max) {
        max = height[i];
      } else {
        trapped += max - height[i];
      }
    }

    return trapped;
  }
};

// *****

int main() {
  return 0;
}
