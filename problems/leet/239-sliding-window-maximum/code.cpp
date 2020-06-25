#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<int> maxSlidingWindow(const vector<int> &nums, int k) {
    int N = nums.size();
    vector<int> answer;
    deque<int> sub;
    answer.reserve(N - k + 1);

    int j = 0;
    while (j < N) {
      while (!sub.empty() && nums[j] >= nums[sub.back()])
        sub.pop_back();
      sub.push_back(j++);
      if (j >= k)
        answer.push_back(nums[sub.front()]);
      if (sub.front() + k <= j)
        sub.pop_front();
    }

    return answer;
  }
};

// *****

int main() {
  return 0;
}
