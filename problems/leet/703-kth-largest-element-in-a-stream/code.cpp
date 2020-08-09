#include <bits/stdc++.h>

using namespace std;

// *****

class KthLargest {
  int k;
  vector<int> heap;

public:
  KthLargest(int q, vector<int> &nums) : k(q) {
    nums.push_back(INT_MIN);
    nth_element(begin(nums), begin(nums) + k, end(nums), greater<int>{});
    heap.insert(end(heap), begin(nums), begin(nums) + k);
    make_heap(begin(heap), end(heap), greater<int>{});
  }

  int add(int val) {
    heap.push_back(val);
    push_heap(begin(heap), end(heap), greater<int>{});
    pop_heap(begin(heap), end(heap), greater<int>{});
    heap.pop_back();
    int kth = heap[0];
    return kth;
  }
};

/**
 * Your KthLargest object will be instantiated and called as such:
 * KthLargest* obj = new KthLargest(k, nums);
 * int param_1 = obj->add(val);
 */

// *****

int main() {
  return 0;
}
