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
  vector<vector<int>> insert(const vector<vector<int>>& intervals,
                             const vector<int>& newInterval) {
    vector<vector<int>> result;
    size_t N = intervals.size();

    int start = newInterval[0], end = newInterval[1];
    if (start == end) return result = intervals;

    size_t i = 0;
    while (i < N && intervals[i][1] < start) result.push_back(intervals[i++]);
    if (i == N) {
      result.push_back({start, end});
      return result;
    }

    int mergeStart = min(start, intervals[i][0]);
    while (i < N && intervals[i][0] <= end) ++i;

    if (i == 0) {
      result.push_back({start, end});
    } else {
      int mergeEnd = max(end, intervals[--i][1]);
      result.push_back({mergeStart, mergeEnd});
      ++i;
    }
    while (i < N) result.push_back(intervals[i++]);

    return result;
  }
};

// *****

void test() {
  Solution S;

  auto res = S.insert({{1, 5}}, {2, 3});

  for (auto inter : res) cout << "[" << inter[0] << ',' << inter[1] << "]  ";
  cout << endl;
}

// *****

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  cout.tie(nullptr);
  test();
  return 0;
}
