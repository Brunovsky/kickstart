
#include <bits/stdc++.h>

using namespace std;
using u64 = uint64_t;
using i64 = int64_t;

// *****

struct Cafe {
  int x, y;
};

struct Query {
  int xmin, xmax, ymin, ymax;
};

inline bool cafe_x_sort(const Cafe& lhs, const Cafe& rhs) {
  return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
}

inline bool cafe_y_sort(const Cafe& lhs, const Cafe& rhs) {
  return lhs.y < rhs.y || (lhs.y == rhs.y && lhs.x < rhs.x);
}

inline bool ordered(int a, int b, int c) {
  return a <= b && b <= c;
}

// O = number of children
// P = smallest power of two at least O.

struct yNode {
  int y, y_right;
  int O;
  Cafe* cafe;
};

struct xNode {
  int x, x_right;
  int O, P;
  vector<yNode> y;
};

inline bool operator<(const yNode& lhs, const yNode& rhs) {
  assert(lhs.cafe != nullptr && rhs.cafe != nullptr);
  return cafe_y_sort(*lhs.cafe, *rhs.cafe);
}

// *****

int N, M, P;
constexpr bool T = true, F = false;
vector<Cafe> cafes;
vector<xNode> tree;

void pushup(int i, int j) {
  if (j >= tree[i].O) return;
  pushup(i, j << 1);
  pushup(i, j << 1 | 1);

  int lO = tree[i].y[j << 1].O, rO = tree[i].y[j << 1 | 1].O;

  yNode ynode;
  ynode.y = tree[i].y[j << 1].y_right;
  ynode.y_right = tree[i].y[j << 1 | 1].y_right;
  ynode.O = lO + rO;

  tree[i].y[j] = move(ynode);
}

void pushup(int i) {
  if (i >= N) return;
  pushup(i << 1);
  pushup(i << 1 | 1);

  int lO = tree[i << 1].O, rO = tree[i << 1 | 1].O;

  xNode xnode;
  xnode.x = tree[i << 1].x_right;
  xnode.x_right = tree[i << 1 | 1].x_right;
  xnode.O = lO + rO;

  xnode.y.assign(2 * xnode.O, {});

  xnode.P = 1;
  while (xnode.P < xnode.O) xnode.P <<= 1;

  auto lbegin = tree[i << 1].y.begin() + lO;
  auto lend = tree[i << 1].y.end();
  auto rbegin = tree[i << 1 | 1].y.begin() + rO;
  auto rend = tree[i << 1 | 1].y.end();
  auto obegin = xnode.y.begin() + xnode.O;
  auto oend = xnode.y.end();

  copy(lbegin, lend, xnode.y.begin() + xnode.O);
  copy(rbegin, rend, xnode.y.begin() + xnode.O + lO);
  sort(obegin, oend);
  rotate(obegin, obegin + (2 * xnode.O - xnode.P), oend);

  tree[i] = move(xnode);

  pushup(i, 1);
}

void prepare() {
  for (int i = 0, n = N; i < N; ++i, ++n) {
    xNode xnode;
    xnode.x = xnode.x_right = cafes[i].x;
    xnode.O = xnode.P = 1;

    yNode ynode;
    ynode.y = ynode.y_right = cafes[i].y;
    ynode.cafe = &cafes[i];
    ynode.O = 1;

    xnode.y.resize(2, {});
    xnode.y[1] = ynode;

    tree[n] = move(xnode);
  }

  auto nbegin = tree.begin() + N;
  auto nend = tree.end();

  rotate(nbegin, nbegin + (2 * N - P), nend);
  pushup(1);
}

u64 count_query_top(int i, Query box);
u64 count_query_left(int i, Query box);
u64 count_query_right(int i, Query box);
u64 count_query_top(int i, int j, Query box);
u64 count_query_left(int i, int j, Query box);
u64 count_query_right(int i, int j, Query box);

u64 count_query_top(int i, int j, Query box) {
  if (i >= N || j >= tree[i].O) {
    return ordered(box.ymin, tree[i].y[j].y, box.ymax);
  }
  // ===
  // go left
  if (box.ymax < tree[i].y[j].y) {
    return count_query_top(i, j << 1, box);
  }
  // go right
  else if (box.ymin > tree[i].y[j].y) {
    return count_query_top(i, j << 1 | 1, box);
  }
  // found y_split
  else {
    return count_query_left(i, j << 1, box) + count_query_right(i, j << 1 | 1, box);
  }
}

u64 count_query_left(int i, int j, Query box) {
  if (j >= tree[i].O) {
    return ordered(box.ymin, tree[i].y[j].y, box.ymax);
  }
  // on the left branch of y_split's subtree
  if (box.ymin <= tree[i].y[j].y) {
    return count_query_left(i, j << 1, box) + tree[i].y[j << 1 | 1].O;
  } else {
    return count_query_left(i, j << 1 | 1, box);
  }
}

u64 count_query_right(int i, int j, Query box) {
  if (j >= tree[i].O) {
    return ordered(box.ymin, tree[i].y[j].y, box.ymax);
  }
  // on the right branch of y_split's subtree
  if (box.ymax >= tree[i].y[j].y) {
    return count_query_right(i, j << 1 | 1, box) + tree[i].y[j << 1].O;
  } else {
    return count_query_right(i, j << 1, box);
  }
}

u64 count_query_top(int i, Query box) {
  if (i >= N) {
    return ordered(box.xmin, tree[i].x, box.xmax) &&
      ordered(box.ymin, tree[i].y[1].y, box.ymax);
  }
  // ===
  // go left
  if (box.xmax < tree[i].x) {
    return count_query_top(i << 1, box);
  }
  // go right
  else if (box.xmin > tree[i].x) {
    return count_query_top(i << 1 | 1, box);
  }
  // found x_split
  else {
    return count_query_left(i << 1, box) + count_query_right(i << 1 | 1, box);
  }
}

u64 count_query_left(int i, Query box) {
  if (i >= N) {
    return ordered(box.xmin, tree[i].x, box.xmax) &&
      ordered(box.ymin, tree[i].y[1].y, box.ymax);
  }
  // on the left branch of x_split's subtree
  if (box.xmin <= tree[i].x) {
    return count_query_left(i << 1, box) + count_query_top(i << 1 | 1, 1, box);
  } else {
    return count_query_left(i << 1 | 1, box);
  }
}

u64 count_query_right(int i, Query box) {
  if (i >= N) {
    return ordered(box.xmin, tree[i].x, box.xmax) &&
      ordered(box.ymin, tree[i].y[1].y, box.ymax);
  }
  // on the right branch of x_split's subtree
  if (box.xmax >= tree[i].x) {
    return count_query_right(i << 1 | 1, box) + count_query_top(i << 1, 1, box);
  } else {
    return count_query_right(i << 1, box);
  }
}

// *****

// Q1: number of cafes P such that P.x > C.x and P.y > C.y.
// Q2: number of cafes P such that P.x < C.x and P.y > C.y.
// Q3: number of cafes P such that P.x < C.x and P.y < C.y.
// Q4: number of cafes P such that P.x > C.x and P.y < C.y.

//  - - - -1 - - - -
//         ^
//     Q2  |  Q1
//         |
// -1 -----+-----> M
//         |
//     Q3  |  Q4
//         |
//  - - - -1 - - - -

u64 compute(int cafe) {
  Cafe& C = cafes[cafe];

  Query box1 {C.x + 1, M, C.y + 1, M};
  Query box2 {-1, C.x - 1, C.y + 1, M};
  Query box3 {-1, C.x - 1, -1, C.y - 1};
  Query box4 {C.x + 1, M, -1, C.y - 1};

  u64 Q1 = count_query_top(1, box1);
  u64 Q2 = count_query_top(1, box2);
  u64 Q3 = count_query_top(1, box3);
  u64 Q4 = count_query_top(1, box4);

  return Q1 * Q3 + Q2 * Q4;
}

auto solve() {
  sort(cafes.begin(), cafes.end(), cafe_x_sort);
  tree.assign(2 * N, {});
  prepare();

  u64 exceptions = 0, V = u64(N);
  for (int i = 0; i < N; ++i) exceptions += compute(i);

  return (V * (V - 1) * (V - 2) / 6) - exceptions;
}

// *****

void reparse_test() {
  i64 V, H, A, B, C, D, E, F;
  cin >> N >> V >> H >> A >> B >> C >> D >> E >> F >> M >> ws;
  cafes.assign(N, {});
  cafes[0] = {int(V), int(H)};
  for (int n = 1; n < N; ++n) {
    int x = (A * V + B * H + C) % i64(M);
    int y = (D * V + E * H + F) % i64(M);
    cafes[n] = {x, y};
    V = x;
    H = y;
  }
  P = 1;
  while (P < N) P <<= 1;
}

// *****

int main() {
  unsigned T;
  cin >> T >> ws;
  for (unsigned t = 1; t <= T; ++t) {
    reparse_test();
    auto solution = solve();
    cout << "Case #" << t << ": " << solution << '\n';
  }
  return 0;
}
