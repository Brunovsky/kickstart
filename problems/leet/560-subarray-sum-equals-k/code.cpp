#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int subarraySum(vector<int> &nums, int k) {
    unordered_map<int, int> prefix_counts;
    prefix_counts[0] = 1;
    int sum = 0;
    int total = 0;
    int N = nums.size();
    for (int i = 0; i < N; i++) {
      sum += nums[i];
      total += prefix_counts[sum - k];
      prefix_counts[sum]++;
    }
    return total;
  }
};

// *****

int main() {
  return 0;
}
