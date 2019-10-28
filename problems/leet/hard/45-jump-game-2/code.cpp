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
  int jump(const vector<int>& nums) {
    if (nums.size() <= 1ul) return 0;
    int left = 0, right = 1, last_index = nums.size() - 1, jumps = 0;

    for (int i = 1; i < last_index; ++i) {
      if (nums[i] - nums[left] + i >= nums[right] - nums[left] + right) {
        right = i;
      }
      if (i - left == nums[left]) {
        left = right;
        right = left + 1;
        ++jumps;
      }
    }

    if (left != last_index) ++jumps;
    return jumps;
  }
};

// *****

void test() {
  Solution S;

  cout << S.jump({2, 3, 1}) << endl;
  cout << S.jump({1, 2}) << endl;
  cout << S.jump({2, 3, 1, 1, 4}) << endl;
  cout << S.jump({2, 3, 1, 1, 1, 4}) << endl;
  cout << S.jump({9}) << endl;
  cout << S.jump({2, 0, 7, 1, 2, 3, 4, 5, 6, 7}) << endl;
  cout << S.jump({2, 0, 7, 1, 2, 3, 4, 5, 6, 7, 8}) << endl;
  cout << S.jump({1, 1, 1, 1}) << endl;
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  test();
  return 0;
}
