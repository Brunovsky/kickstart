#include <bits/stdc++.h>

using namespace std;

// *****

// slope = num / den
struct line_t {
  int num, den;

  friend inline bool operator==(const line_t &lhs, const line_t &rhs) {
    return tie(lhs.num, lhs.den) == tie(rhs.num, rhs.den);
  }

  friend inline bool operator<(const line_t &lhs, const line_t &rhs) {
    return tie(lhs.num, lhs.den) < tie(rhs.num, rhs.den);
  }
};

namespace std {

template <>
struct hash<line_t> {
  size_t operator()(const line_t &line) const {
    size_t seed = 0x89421bfa;
    seed ^= line.num + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    seed ^= line.den + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};

} // namespace std

class Solution {
public:
  int maxPoints(const vector<vector<int>> &points) {
    if (points.empty())
      return 0;

    int best_count = 0;

    for (size_t i = 0; i < points.size(); ++i) {
      unordered_map<line_t, int> counts;
      // map<line_t, int> counts;

      int repeat = 1;

      for (size_t j = i + 1; j < points.size(); ++j) {
        int Ax = points[i][0], Ay = points[i][1];
        int Bx = points[j][0], By = points[j][1];
        if (Ax == Bx && Ay == By) {
          ++repeat;
          continue;
        }
        if (Ax < Bx)
          swap(Ax, Bx), swap(Ay, By);

        int Dx = Ax - Bx, Dy = Ay - By;
        int sgcd = gcd(Dy, Dx);
        line_t line = {Dy / sgcd, Dx / sgcd};

        if (Dx == 0)
          line.num = abs(line.num);

        ++counts[line];
      }

      int c = 0;
      for (const auto &[_, count] : counts)
        if (c < count)
          c = count;

      if (best_count < c + repeat)
        best_count = c + repeat;

      int n = points.size() - i;
      if (n * (n - 1) < 2 * best_count)
        break;
    }

    return best_count;
  }
};

// *****

int main() {
  const vector<vector<vector<int>>> tests = {
      {{1, 1}, {2, 2}, {3, 3}},
      {{1, 1}, {3, 2}, {5, 3}, {4, 1}, {2, 3}, {1, 4}},
      {{0, 0}, {0, 0}},
      {{0, 0}, {1, 1}, {0, 0}},
      {{4, 5}, {4, -1}, {4, 0}},
      {{0, -1},
       {0, 3},
       {0, -4},
       {0, -2},
       {0, -4},
       {0, 0},
       {0, 0},
       {0, 1},
       {0, -2},
       {0, 4}},
  };

  const vector<int> expected = {3, 4, 2, 3, 3, 10};

  for (size_t i = 0; i < tests.size(); ++i) {
    int actual = Solution{}.maxPoints(tests[i]);
    if (actual == expected[i])
      continue;
    printf("Test #%lu failed\n", i);
    printf("> Expected: %d\n", expected[i]);
    printf("> Actual:   %d\n", actual);
  }

  return 0;
}
