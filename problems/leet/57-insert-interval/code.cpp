#include <bits/stdc++.h>

using namespace std;

// *****

class Solution {
public:
  vector<vector<int>> insert(const vector<vector<int>> &intervals,
                             const vector<int> &newInterval) {
    vector<vector<int>> result;
    size_t N = intervals.size();

    int start = newInterval[0], end = newInterval[1];
    if (start == end)
      return result = intervals;

    size_t i = 0;
    while (i < N && intervals[i][1] < start)
      result.push_back(intervals[i++]);
    if (i == N) {
      result.push_back({start, end});
      return result;
    }

    int mergeStart = min(start, intervals[i][0]);
    while (i < N && intervals[i][0] <= end)
      ++i;

    if (i == 0) {
      result.push_back({start, end});
    } else {
      int mergeEnd = max(end, intervals[--i][1]);
      result.push_back({mergeStart, mergeEnd});
      ++i;
    }
    while (i < N)
      result.push_back(intervals[i++]);

    return result;
  }
};

// *****

int main() {
  return 0;
}
