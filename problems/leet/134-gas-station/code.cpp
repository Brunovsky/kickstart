#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  int canCompleteCircuit(const vector<int> &gas, const vector<int> &cost) {
    int lowest = INT_MAX, sum = 0, index = 0, N = gas.size();
    for (int i = 0; i < N; ++i) {
      sum += gas[i] - cost[i];
      if (sum < lowest) {
        index = i;
        lowest = sum;
      }
    }
    if (sum < 0)
      return -1;
    if (index + 1 == N)
      return 0;
    return index + 1;
  }
};

// *****

int main() {
  return 0;
}
