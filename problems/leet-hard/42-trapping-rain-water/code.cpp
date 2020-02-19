#include <bits/stdc++.h>

using namespace std;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

// *****

class Solution {
 public:
  int trap(const vector<int>& height) {
    if (height.size() <= 2) return 0;

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

void test() {
  cout << "Test: " << Solution().trap({0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1}) << '\n';
}

// *****

int main() {
  ios::sync_with_stdio(false);
  test();
  return 0;
}
