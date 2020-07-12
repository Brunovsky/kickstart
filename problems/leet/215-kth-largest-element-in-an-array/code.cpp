#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int findKthLargest(vector<int> &nums, int k) {
    vector<int> heap;
    heap.reserve(k + 1);

    for (int i = 0; i < k; i++) {
      heap.push_back(nums[i]);
    }
    make_heap(heap.begin(), heap.end(), greater<int>{});

    for (int i = k, N = nums.size(); i < N; i++) {
      heap.push_back(nums[i]);
      push_heap(heap.begin(), heap.end(), greater<int>{});
      pop_heap(heap.begin(), heap.end(), greater<int>{});
      heap.pop_back();
    }

    return heap[0];
  }
};

// *****

int main() {
  return 0;
}
