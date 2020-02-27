#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int firstMissingPositive(vector<int> &nums) {
    int size = nums.size();

    for (int i = 0; i < size; ++i) {
      int prev = ~nums[i];
      while (nums[i] != i + 1 && nums[i] > 0 && nums[i] <= size &&
             nums[i] != prev) {
        prev = nums[i];
        swap(nums[i], nums[nums[i] - 1]);
      }
    }

    for (int i = 0; i < size; ++i) {
      if (nums[i] != i + 1)
        return i + 1;
    }

    return size + 1;
  }
};

// *****

void test() {
  Solution S;

  vector<int> nums1{1, 2, 0};
  vector<int> nums2{3, 4, -1, 1};
  vector<int> nums3{7, 8, 9, 11, 12};
  vector<int> nums4{1};
  vector<int> nums5{2};
  vector<int> nums6{4, 6, 2, 7, 1, 5, 3};
  vector<int> nums7{2, 4, 3, 7, 6, 3};

  cout << "3 -> " << S.firstMissingPositive(nums1) << endl;
  cout << "2 -> " << S.firstMissingPositive(nums2) << endl;
  cout << "1 -> " << S.firstMissingPositive(nums3) << endl;
  cout << "2 -> " << S.firstMissingPositive(nums4) << endl;
  cout << "1 -> " << S.firstMissingPositive(nums5) << endl;
  cout << "8 -> " << S.firstMissingPositive(nums6) << endl;
  cout << "1 -> " << S.firstMissingPositive(nums7) << endl;
}

// *****

int main() {
  test();
  return 0;
}
