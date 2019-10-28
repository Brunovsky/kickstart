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
  int firstMissingPositive(vector<int>& nums) {
    int size = nums.size();

    for (int i = 0; i < size; ++i) {
      int prev = ~nums[i];
      while (nums[i] != i + 1 && nums[i] > 0 && nums[i] <= size && nums[i] != prev) {
        prev = nums[i];
        swap(nums[i], nums[nums[i] - 1]);
      }
    }

    for (int i = 0; i < size; ++i) {
      if (nums[i] != i + 1) return i + 1;
    }

    return size + 1;
  }
};

// *****

void test() {
  Solution S;

  vector<int> nums1 {1, 2, 0};
  vector<int> nums2 {3, 4, -1, 1};
  vector<int> nums3 {7, 8, 9, 11, 12};
  vector<int> nums4 {1};
  vector<int> nums5 {2};
  vector<int> nums6 {4, 6, 2, 7, 1, 5, 3};
  vector<int> nums7 {2, 4, 3, 7, 6, 3};

  cout << S.firstMissingPositive(nums1) << endl;
  cout << S.firstMissingPositive(nums2) << endl;
  cout << S.firstMissingPositive(nums3) << endl;
  cout << S.firstMissingPositive(nums4) << endl;
  cout << S.firstMissingPositive(nums5) << endl;
  cout << S.firstMissingPositive(nums6) << endl;
  cout << S.firstMissingPositive(nums7) << endl;
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  test();
  return 0;
}
