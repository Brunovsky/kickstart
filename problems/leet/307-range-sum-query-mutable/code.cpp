#include <bits/stdc++.h>

using namespace std;

// *****

class NumArray {
  int N;
  vector<int> fenwick;
  vector<int> &nums;

public:
  NumArray(vector<int> &nums) : nums(nums) {
    N = nums.size();
    fenwick.assign(N + 1, 0);
    for (int i = 0; i < N; i++)
      add(i + 1, nums[i]);
  }

  void add(int i, int n) {
    while (i <= N) {
      fenwick[i] += n;
      i += i & -i;
    }
  }

  int sum(int i) const {
    int ans = 0;
    while (i > 0) {
      ans += fenwick[i];
      i -= i & -i;
    }
    return ans;
  }

  void update(int i, int val) {
    int n = val - nums[i];
    nums[i] = val;
    add(i + 1, n);
  }

  int sumRange(int i, int j) const {
    return sum(j + 1) - sum(i);
  }
};

// *****

int main() {
  vector<int> nums = {1, 3, 5};
  NumArray arr(nums);
  cout << arr.sumRange(0, 2) << endl;
  arr.update(1, 2);
  cout << arr.sumRange(0, 2) << endl;
  return 0;
}
