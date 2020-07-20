#include <bits/stdc++.h>

using namespace std;

// *****

constexpr size_t MAX_SIZE_LEAF = 4;

struct Point {
  int x, y;
};

struct Box {
  Point min, max;
};

using PointIt = vector<Point>::iterator;

inline bool operator==(Point lhs, Point rhs) {
  return tie(lhs.x, lhs.y) == tie(rhs.x, rhs.y);
}

inline bool operator<(Point lhs, Point rhs) {
  return tie(lhs.x, lhs.y) < tie(rhs.x, rhs.y);
}

inline bool ordered(Point lo, Point hi) {
  return lo.x <= hi.x && lo.y <= hi.y;
}

inline bool bounded(Box box, Point point) {
  return ordered(box.min, point) && ordered(point, box.max);
}

inline bool intersect(Box aa, Box bb) {
  return ordered(aa.min, bb.max) && ordered(bb.min, aa.max);
}

inline bool contains(Box large, Box small) {
  return ordered(large.min, small.min) && ordered(small.max, large.max);
}

bool cmp_x(Point lhs, Point rhs) {
  return lhs.x < rhs.x;
}

bool cmp_y(Point lhs, Point rhs) {
  return lhs.y < rhs.y;
}

PointIt partition_x(PointIt first, PointIt last, int x) {
  while (first != last && first->x < x)
    ++first;
  if (first == last)
    return first;

  for (auto it = next(first); it != last; ++it) {
    if (it->x < x) {
      iter_swap(it, first);
      ++first;
    }
  }
  return first;
}

PointIt partition_y(PointIt first, PointIt last, int y) {
  while (first != last && first->y < y)
    ++first;
  if (first == last)
    return first;

  for (auto it = next(first); it != last; ++it) {
    if (it->y < y) {
      iter_swap(it, first);
      ++first;
    }
  }
  return first;
}

struct quad_tree {
  unique_ptr<quad_tree> children[4];
  Box box;
  PointIt first, last;

  quad_tree(PointIt first, PointIt last) : first(first), last(last) {
    box = {{INT_MAX, INT_MAX}, {INT_MIN, INT_MIN}};
    for (auto it = first; it != last; ++it) {
      box.min.x = min(box.min.x, it->x);
      box.min.y = min(box.min.y, it->y);
      box.max.x = max(box.max.x, it->x);
      box.max.y = max(box.max.y, it->y);
    }
    size_t size = last - first;

    if (size <= MAX_SIZE_LEAF) {
      return;
    }

    auto middle = size / 2;
    Point root;
    nth_element(first, first + middle, last, cmp_x);
    root.x = (first + middle)->x;
    nth_element(first, first + middle, last, cmp_y);
    root.y = (first + middle)->y;

    if (*first == root) {
      return;
    }

    auto nth2 = partition_x(first, last, root.x);
    auto nth1 = partition_y(first, nth2, root.y);
    auto nth3 = partition_y(nth2, last, root.y);

    children[0] = make_unique<quad_tree>(first, nth1);
    children[1] = make_unique<quad_tree>(nth1, nth2);
    children[2] = make_unique<quad_tree>(nth2, nth3);
    children[3] = make_unique<quad_tree>(nth3, last);
  }

  size_t count_query(Box query) const noexcept {
    size_t size = last - first;
    if (size == 0 || !intersect(query, box)) {
      return 0;
    }
    if (contains(query, box)) {
      return size;
    }
    size_t count = 0;
    if (!children[0]) {
      if (size <= MAX_SIZE_LEAF) {
        for (auto it = first; it != last; ++it) {
          count += bounded(query, *it);
        }
      } else {
        count = size * bounded(query, *first);
      }
    } else {
      count += children[0]->count_query(query);
      count += children[1]->count_query(query);
      count += children[2]->count_query(query);
      count += children[3]->count_query(query);
    }
    return count;
  }
};

// *****

long N, M;
vector<Point> cafes;

void read() {
  long X0, Y0, A, B, C, D, E, F;
  cin >> N >> X0 >> Y0 >> A >> B >> C >> D >> E >> F >> M >> ws;
  cafes.resize(N);
  cafes[0] = {int(X0), int(Y0)};
  for (long i = 1; i < N; i++) {
    long x = (A * X0 + B * Y0 + C) % M;
    long y = (D * X0 + E * Y0 + F) % M;
    cafes[i] = {int(x), int(y)};
    X0 = x, Y0 = y;
  }
}

auto solve() {
  read();
  quad_tree tree(cafes.begin(), cafes.end());

  size_t total = N * (N - 1) * (N - 2) / 6; // N choose 3
  int MIN = -1, MAX = M;
  for (auto cafe : cafes) {
    size_t Q0 = tree.count_query({{MIN, MIN}, {cafe.x - 1, cafe.y - 1}});
    size_t Q1 = tree.count_query({{MIN, cafe.y + 1}, {cafe.x - 1, MAX}});
    size_t Q2 = tree.count_query({{cafe.x + 1, MIN}, {MAX, cafe.y - 1}});
    size_t Q3 = tree.count_query({{cafe.x + 1, cafe.y + 1}, {MAX, MAX}});
    size_t exceptions = Q0 * Q3 + Q1 * Q2;
    total -= exceptions;
  }
  return total;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
