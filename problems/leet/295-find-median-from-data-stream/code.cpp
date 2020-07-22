#include <bits/stdc++.h>

using namespace std;

// *****

class MedianFinder {
  vector<int> lower, higher;

public:
  /** initialize your data structure here. */
  MedianFinder() {}

  void addNum(int num) {
    if (lower.empty() || num <= lower[0]) {
      lower.push_back(num);
      push_heap(begin(lower), end(lower), less<int>{});
      if (lower.size() > higher.size() + 1) {
        pop_heap(begin(lower), end(lower), less<int>{});
        higher.push_back(lower.back());
        lower.pop_back();
        push_heap(begin(higher), end(higher), greater<int>{});
      }
    } else {
      higher.push_back(num);
      push_heap(begin(higher), end(higher), greater<int>{});
      if (lower.size() + 1 < higher.size()) {
        pop_heap(begin(higher), end(higher), greater<int>{});
        lower.push_back(higher.back());
        higher.pop_back();
        push_heap(begin(lower), end(lower), less<int>{});
      }
    }
  }

  double findMedian() {
    if (lower.size() > higher.size()) {
      return lower[0];
    } else if (lower.size() < higher.size()) {
      return higher[0];
    } else {
      assert(lower.size() && higher.size());
      return 0.5 * (lower[0] + higher[0]);
    }
  }
};

// *****

int main() {
  return 0;
}
