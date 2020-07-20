#include <bits/stdc++.h>

using namespace std;

constexpr size_t MAX_SZ_LEAF = 4;

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

inline bool operator!=(Point lhs, Point rhs) {
  return tie(lhs.x, lhs.y) != tie(rhs.x, rhs.y);
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
  array<unique_ptr<quad_tree>, 4> children;
  Box box;
  size_t size = 0;
  vector<Point> points;

  bool is_partitioned() const noexcept {
    return bool(children[0]);
  }

  bool is_singular() const noexcept {
    return box.min == box.max;
  }

  Point root_point() const noexcept {
    return {(box.min.x + box.max.x + 1) >> 1, (box.min.y + box.max.y + 1) >> 1};
  }

  void partition_tree() noexcept {
    if (is_partitioned()) {
      return;
    }
    assert(!is_singular());
    auto root = root_point();
    Box box0 = {{box.min.x, box.min.y}, {root.x - 1, root.y - 1}};
    Box box1 = {{box.min.x, root.y}, {root.x - 1, box.max.y}};
    Box box2 = {{root.x, box.min.y}, {box.max.x, root.y - 1}};
    Box box3 = {{root.x, root.y}, {box.max.x, box.max.y}};
    children[0] = make_unique<quad_tree>(box0);
    children[1] = make_unique<quad_tree>(box1);
    children[2] = make_unique<quad_tree>(box2);
    children[3] = make_unique<quad_tree>(box3);

    partition_insert(begin(points), end(points));
    points.clear();
  }

  void partition_insert(PointIt first, PointIt last) {
    auto root = root_point();
    auto nth2 = partition_x(first, last, root.x);
    auto nth1 = partition_y(first, nth2, root.y);
    auto nth3 = partition_y(nth2, last, root.y);
    children[0]->insert(first, nth1);
    children[1]->insert(nth1, nth2);
    children[2]->insert(nth2, nth3);
    children[3]->insert(nth3, last);
  }

  quad_tree(Box box) : box(box), size(0) {}

  void clear() noexcept {
    children[0] = nullptr;
    children[1] = nullptr;
    children[2] = nullptr;
    children[3] = nullptr;
    size = 0;
    points.clear();
  }

  void insert(PointIt first, PointIt last) {
    if (first == last) {
      return;
    }
    size += last - first;

    if (is_singular()) {
      return;
    }

    if (!is_partitioned() && size <= MAX_SZ_LEAF) {
      copy(first, last, back_inserter(points));
      return;
    }

    if (!is_partitioned()) {
      partition_tree();
    }

    partition_insert(first, last);
  }

  size_t count_query(Point query) const noexcept {
    if (size == 0 || !ordered(box.min, query)) {
      return 0;
    }
    if (ordered(box.max, query)) {
      return size;
    }
    size_t count = 0;
    if (!is_partitioned()) {
      for (auto point : points) {
        count += ordered(point, query);
      }
    } else {
      count += children[0]->count_query(query);
      count += children[1]->count_query(query);
      count += children[2]->count_query(query);
      count += children[3]->count_query(query);
    }
    return count;
  }

  size_t tree_size() const noexcept {
    return size;
  }
};

// *****

// (3N choose N)
constexpr size_t choose2N[] = {1, 2, 6, 20, 70, 252};
constexpr size_t choose3N[] = {1, 3, 15, 84, 495, 3003};

#define MAXSUM 8388608
#define F2 0
#define F12 1

using SumMask = pair<int, int>;

// When it doubt, it's a fenwick tree. Even if you doubt it's a fenwick tree!
int SUP1MASK;
int SUP2MASK;
int NMASK;
int N;
int fenwick[MAXSUM];
quad_tree a23tree({{0, 0}, {MAXSUM, MAXSUM}});
unordered_map<int, unordered_map<int, int>> dp12;
unordered_map<int, unordered_map<int, int>> dp23;

int Asum, Bsum;
vector<int> A;
vector<int> B;

inline int next_lexicographical_mask(int v) {
  int t = v | (v - 1);
  int w = (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
  assert(__builtin_popcount(v) == N);
  return w;
}

inline int sum_fenwick(int i) {
  int sum = 0;
  while (i > 0) {
    sum += fenwick[i];
    i -= i & -i;
  }
  return sum;
}

inline void add_fenwick(int i) {
  while (i < MAXSUM) {
    fenwick[i]++;
    i += i & -i;
  }
}

int sum_mask(int mask, const vector<int> &V) {
  int i = 0, sum = 0;
  while (mask) {
    if (mask & 1) {
      sum += V[i];
    }
    i++, mask >>= 1;
  }
  return sum;
}

vector<SumMask> makesums(const vector<int> &V) {
  vector<SumMask> summasks;
  int mask = NMASK;
  do {
    summasks.push_back({sum_mask(mask, V), mask});
    mask = next_lexicographical_mask(mask);
  } while (mask < SUP1MASK);
  sort(begin(summasks), end(summasks));
  return summasks;
}

auto remaining_cards(int mask, const vector<int> &V) {
  vector<int> remaining;
  remaining.reserve(2 * N);
  for (int i = 0; i < 3 * N; i++) {
    if (!(mask & (1 << i))) {
      remaining.push_back(V[i]);
    }
  }
  return remaining;
}

void read() {
  memset(fenwick, 0, sizeof(fenwick));
  a23tree.clear();
  dp12.clear();
  dp23.clear();
  cin >> N >> ws;
  A.resize(3 * N + 1);
  B.resize(3 * N + 1);
  for (int i = 0; i < 3 * N; i++) {
    cin >> B[i] >> ws;
  }
  for (int i = 0; i < 3 * N; i++) {
    cin >> A[i] >> ws;
  }
  A[3 * N] = B[3 * N] = 0; // to allow masks larger than 1 << 3N
  Asum = accumulate(begin(A), end(A), 0);
  Bsum = accumulate(begin(B), end(B), 0);
}

auto solve() {
  read();
  SUP1MASK = 1 << (3 * N);
  SUP2MASK = 1 << (2 * N);
  NMASK = (1 << N) - 1;

  auto a1summasks = makesums(A);
  auto b1summasks = makesums(B);
  size_t a = 0;

  for (SumMask b1sm : b1summasks) {
    int B1 = b1sm.first;
    int b1mask = b1sm.second;
    auto Bremaining = remaining_cards(b1mask, B);

    vector<Point> points;
    // advance A
    while (a < choose3N[N] && a1summasks[a].first < B1) {
      int A1 = a1summasks[a].first;
      int a1mask = a1summasks[a].second;
      auto Aremaining = remaining_cards(a1mask, A);
      a++;

      points.reserve(points.size() + choose2N[N]);

      int a2mask = NMASK;
      do {
        int A2 = sum_mask(a2mask, Aremaining);
        int A3 = Asum - (A1 + A2);
        add_fenwick(A2);
        points.push_back({A2, A3});
        a2mask = next_lexicographical_mask(a2mask);
      } while (a2mask < SUP2MASK);
    }
    a23tree.insert(begin(points), end(points));

    int b2mask = NMASK;
    do {
      int B2 = sum_mask(b2mask, Bremaining);
      int B3 = Bsum - (B1 + B2);
      // [A1 < B1] | A2 < B2 ==> A2 <= B2 - 1
      // [A1 < B1] | A2 < B2 && A3 < B3 ==> ordered({A2, A3}, {B2-1, B3-1});
      if (B1 >= B2) {
        int size12 = sum_fenwick(B2 - 1);
        dp12[B1][B2] = size12;
        assert(size12 >= 0);

        if (B2 >= B3) {
          int size23 = a23tree.count_query({B2 - 1, B3 - 1});
          dp23[B2][B3] = size23;
          assert(size23 >= 0);
        }
      }
      b2mask = next_lexicographical_mask(b2mask);
    } while (b2mask < SUP2MASK);
  }

  int max_beaten = 0;

  for (SumMask b1sm : b1summasks) {
    int B1 = b1sm.first;
    int b1mask = b1sm.second;
    auto Bremaining = remaining_cards(b1mask, B);

    int b2mask = NMASK;
    do {
      int B2 = sum_mask(b2mask, Bremaining);
      int B3 = Bsum - (B1 + B2);
      if (B1 >= B2 && B2 >= B3) {
        int size_12 = dp12[B1][B2];
        int size_13 = dp12[B1][B3];
        int size_23 = dp12[B2][B3];
        int size_123 = dp23[B2][B3];
        int beaten = (size_12 + size_13 + size_23) - 2 * size_123;
        max_beaten = max(max_beaten, beaten);
      }
      b2mask = next_lexicographical_mask(b2mask);
    } while (b2mask < SUP2MASK);
  }

  return max_beaten / double(choose3N[N] * choose2N[N]);
}

// *****

int main() {
  unsigned T;
  cout << fixed << showpoint << setprecision(8);
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
