#include <bits/stdc++.h>

using namespace std;

// *****

using Point = vector<int>;

namespace std {
template <>
struct hash<Point> {
  static constexpr size_t len = 2;
  size_t operator()(const Point &point) const noexcept {
    assert(point.size() == 2);
    size_t x = point[0], y = point[1];
    return hash<size_t>()(x ^ (y << 32));
  }
};
} // namespace std

int distance_between(const Point &p1, const Point &p2) {
  return abs(p1[0] - p2[0]) + abs(p1[1] - p2[1]);
}

bool out_of_bounds(const Point &point) {
  return point[0] < 0 || point[0] > 999999 || point[1] < 0 || point[1] > 999999;
}

class Solution {
public:
  bool isEscapePossible(const vector<Point> &blocked, const Point &source,
                        const Point &target) {
    if (out_of_bounds(target))
      return false;

    unordered_set<Point> blocked_set;
    for (const Point &point : blocked)
      blocked_set.insert(point);

    if (blocked_set.count(target))
      return false;

    vector<Point> frontier, old_frontier;
    frontier.push_back(source);
    int d = 0;

    const auto add = [&](const Point &point) {
      if (!out_of_bounds(point) && distance_between(point, source) == d &&
          !blocked_set.count(point))
        frontier.push_back(point);
    };

    while (!frontier.empty()) {
      ++d;
      swap(frontier, old_frontier);
      frontier.clear();

      for (Point &point : old_frontier) {
        // printf("(%d, %d) ", point[0], point[1]);
        point[0] += 1;
        add(point);
        point[0] -= 2;
        add(point);

        point[0] += 1;

        point[1] += 1;
        add(point);
        point[1] -= 2;
        add(point);
      }
      // printf("\n");

      for (const Point &point : frontier)
        if (point == target)
          return true;
    }

    return false;
  }
};

// *****

/**
 * 0 0 0 0
 * 0 1 1 0
 * 0 1 0 0
 * 0 1 0 1
 * 0 1 0 1
 * 1 0 1 1
 */

int main() {
  const vector<Point> blocked = {
      {1, 1}, {1, 2}, {2, 1}, {3, 1}, {3, 3}, {4, 1},
      {4, 3}, {5, 0}, {5, 3}, {5, 2}, {6, 1},
  };
  bool ok = Solution{}.isEscapePossible(blocked, {0, 0}, {5, 1});

  return 0;
}
